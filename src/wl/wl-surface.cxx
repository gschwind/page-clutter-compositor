/*
 * Copyright (2016) Benoit Gschwind
 *
 * surface.cxx is part of page-compositor.
 *
 * page-compositor is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * page-compositor is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with page-compositor.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#include "wl-surface.hxx"

#include <algorithm>
#include <limits>
#include <cassert>
#include <wayland-server-protocol.h>

#include "utils.hxx"
#include "wl-compositor.hxx"
#include "wl-buffer.hxx"
#include "wl-region.hxx"
#include "wl-callback.hxx"
#include "wl-subsurface.hxx"

namespace page {
namespace wl {

wl_surface::wl_surface(wl_compositor * compositor, struct wl_client *client, uint32_t version, uint32_t id) :
	wl_surface_vtable{client, version, id},
	compositor{compositor},
	buffer{nullptr},
	subsurface{nullptr}
{
	/* NOTE: following the source of clutter surface is not used by clutter,
	 * this is just user data that can be retrieved by 	clutter_wayland_stage_get_wl_surface() */
	actor = meta_surface_actor_wayland_new(this);

	buffer = nullptr;

	cairo_region_reference(pending.opaque_region);
	opaque_region = pending.opaque_region;
	cairo_region_reference(pending.input_region);
	input_region = pending.input_region;

}

wl_surface::~wl_surface() {
	// TODO Auto-generated destructor stub
}

wl_surface * wl_surface::get(struct wl_resource *r) {
	return dynamic_cast<wl_surface*>(resource_get<wl_surface_vtable>(r));
}

void wl_surface::commit_state(wl_surface_state & state) {

	if(state.buffer)
		state.buffer->incr_use_count();
	if(buffer)
		buffer->decr_use_count();
	buffer = state.buffer;
	state.buffer = nullptr;

	if(buffer) {
		auto texture = buffer->ensure_texture();
		meta_surface_actor_wayland_set_texture(META_SURFACE_ACTOR_WAYLAND(actor), texture);

		if(state.damage_surface)
			buffer->process_damage(state.damage_surface);

	}

	/* clear damages */
	if(state.damage_surface) {
		cairo_region_destroy(state.damage_surface);
		state.damage_surface = nullptr;
	}

	if(input_region != state.input_region) {
		cairo_region_destroy(input_region);
		input_region = state.input_region;
		cairo_region_reference(input_region);
	}

	if(opaque_region != state.opaque_region) {
		cairo_region_destroy(opaque_region);
		opaque_region = state.opaque_region;
		cairo_region_reference(opaque_region);
	}

	frame_callback_list.splice(frame_callback_list.end(), frame_callback_list,
			state.frame_callback_list.begin(), state.frame_callback_list.end());

	/* this sync the current surface state to the actor */
	meta_surface_actor_wayland_sync_state (META_SURFACE_ACTOR_WAYLAND(actor));

}

void wl_surface::synchronize_subsurface_stack()
{ // resync child stack
	auto iter = subsurface_list.begin();
	for(auto x: subsurface_pending_list) {
		auto pos = std::find(iter, subsurface_list.end(), x);
		if((pos == iter) and (iter != subsurface_list.end())) { /* found at the same position */
			++iter;
			continue;
		} else if(pos == subsurface_list.end()) { // not found
			clutter_actor_insert_child_below(CLUTTER_ACTOR(actor),
					CLUTTER_ACTOR(x->surface->actor),
					CLUTTER_ACTOR((*iter)->surface->actor));
			subsurface_list.insert(iter, x);
		} else { /* found at another position */
			clutter_actor_set_child_below_sibling(CLUTTER_ACTOR(actor),
					CLUTTER_ACTOR(x->surface->actor),
					CLUTTER_ACTOR((*iter)->surface->actor));
			subsurface_list.splice(iter, subsurface_list, pos);
		}
	}
}

void wl_surface::commit_synchronized_state_recursively()
{
	/* select the state to commit */
	if(subsurface) {
		if(subsurface->is_synchronized() and subsurface->has_pending_state) {
			commit_state(subsurface->synchronized_pending_state);
			subsurface->has_pending_state = false;
		}
	} else {
		commit_state(pending);
	}

	for(auto x: subsurface_list) {
		subsurface->surface->commit_synchronized_state_recursively();
	}
}

void wl_surface::recv_destroy(struct wl_client * client, struct wl_resource * resource) {
	wl_resource_destroy(_self_resource);
}

void wl_surface::recv_attach(struct wl_client * client, struct wl_resource * resource, struct wl_resource * buffer_resource, int32_t x, int32_t y) {

	wl_buffer * buffer = nullptr;
	if(buffer_resource) {
		buffer = compositor->ensure_wl_buffer(buffer_resource);
		if (buffer == NULL) {
			wl_client_post_no_memory(client);
			return;
		}
	}

	/* Attach, attach, without commit in between does not send
	 * wl_buffer.release. */
	pending.set_buffer(buffer);

	pending.new_buffer_relative_position_x = x;
	pending.new_buffer_relative_position_y = y;
	pending.newly_attached = true;
}

void wl_surface::recv_damage(struct wl_client * client, struct wl_resource * resource, int32_t x, int32_t y, int32_t width, int32_t height) {

	if (width <= 0 || height <= 0)
		return;

	cairo_rectangle_int_t rect = {x, y, width, height};
	if(not pending.damage_surface)
		pending.damage_surface = cairo_region_create_rectangle(&rect);
	else
		cairo_region_union_rectangle(pending.damage_surface, &rect);

}

void wl_surface::recv_frame(struct wl_client * client, struct wl_resource * resource, uint32_t callback) {

	auto cb = new wl_callback{client, 1, callback};
	if (cb == NULL) {
		wl_resource_post_no_memory(resource);
		return;
	}

	pending.frame_callback_list.push_back(cb);

}

void wl_surface::recv_set_opaque_region(struct wl_client * client, struct wl_resource * resource, struct wl_resource * region_resource) {
	if(pending.opaque_region)
		cairo_region_destroy(pending.opaque_region);
	if (region_resource) {
		auto region = wl_region::get(region_resource);
		pending.opaque_region = cairo_region_copy(region->_region);
	} else {
		pending.opaque_region = cairo_region_create();
	}
}

void wl_surface::recv_set_input_region(struct wl_client * client, struct wl_resource * resource, struct wl_resource * region_resource) {
	if(pending.input_region)
		cairo_region_destroy(pending.input_region);
	if (region_resource) {
		auto region = wl_region::get(region_resource);
		pending.input_region = cairo_region_copy(region->_region);
	} else {
		pending.input_region = cairo_region_create_infini();
	}
}

void wl_surface::recv_commit(struct wl_client * client, struct wl_resource * resource){
	printf("call %s (%p)", __PRETTY_FUNCTION__, this);

	/* this subsurface properties are always commited on commit */
	synchronize_subsurface_stack();

	if(subsurface) {
		subsurface->commit();
		/*
		 * if the subsurface is synchronized, we have to wait for the parent
		 * commit. Else we commit imediatly.
		 */
		if(subsurface->is_synchronized()) {
			subsurface->commit_state(pending);
		} else {
			commit_state(pending);
		}
	} else {

		/* when commited we may need to propagate commit */
		commit_synchronized_state_recursively();
	}

}

void wl_surface::recv_set_buffer_transform(struct wl_client * client, struct wl_resource * resource, int32_t transform) {
	/* if wl_output.transform grows more members this will need to be updated. */
	if (transform < 0 ||
	    transform > WL_OUTPUT_TRANSFORM_FLIPPED_270) {
		wl_resource_post_error(resource,
			WL_SURFACE_ERROR_INVALID_TRANSFORM,
			"buffer transform must be a valid transform "
			"('%d' specified)", transform);
		return;
	}

	// TODO
	//pending.buffer_viewport.buffer.transform = transform;
	//pending.buffer_viewport.changed = 1;
}

void wl_surface::recv_set_buffer_scale(struct wl_client * client, struct wl_resource * resource, int32_t scale) {

	if (scale < 1) {
		wl_resource_post_error(resource,
			WL_SURFACE_ERROR_INVALID_SCALE,
			"buffer scale must be at least one "
			"('%d' specified)", scale);
		return;
	}

	// TODO
	//pending.buffer_viewport.buffer.scale = scale;
	//pending.buffer_viewport.changed = 1;

}

void wl_surface::recv_damage_buffer(struct wl_client * client, struct wl_resource * resource, int32_t x, int32_t y, int32_t width, int32_t height) {

	if (width <= 0 || height <= 0)
		return;

	cairo_rectangle_int_t rect = {x, y, width, height};
	cairo_region_union_rectangle(pending.damage_buffer, &rect);

}

void wl_surface::delete_resource(struct wl_resource * resource){
	delete this;
}

}
} /* namespace page */
