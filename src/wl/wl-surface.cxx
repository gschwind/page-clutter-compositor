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

using namespace std;

wl_surface_state::wl_surface_state() :
		newly_attached{0},
		buffer{nullptr},
		new_buffer_relative_position_x{0},
		new_buffer_relative_position_y{0},
		damage_surface{nullptr},
		damage_buffer{nullptr},
		opaque_region{nullptr},
		input_region{nullptr},
		scale{1}
{

	opaque_region = cairo_region_create();
	input_region = cairo_region_create_infini();

}

void wl_surface_state::on_buffer_destroy(wl_buffer * b) {
	assert(buffer == b);
	buffer = nullptr;
	buffer_destroy_listener.reset();
}

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

void wl_surface::state_set_buffer(wl_surface_state * state, wl_buffer * buffer) {
	if(state->buffer == buffer)
		return;

	if(not buffer) {
		state->buffer = nullptr;
		state->buffer_destroy_listener.reset();
	} else {
		state->buffer = buffer;
		state->buffer_destroy_listener =
				buffer->destroy_signal.connect(&pending, &wl_surface_state::on_buffer_destroy);
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
	state_set_buffer(&pending, buffer);

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

//	struct weston_surface *surface = wl_resource_get_user_data(resource);
//	struct weston_subsurface *sub = weston_surface_to_subsurface(surface);
//
//	if (!weston_surface_is_pending_viewport_source_valid(surface)) {
//		assert(surface->viewport_resource);
//
//		wl_resource_post_error(surface->viewport_resource,
//			WP_VIEWPORT_ERROR_OUT_OF_BUFFER,
//			"wl_surface@%d has viewport source outside buffer",
//			wl_resource_get_id(resource));
//		return;
//	}
//
//	if (!weston_surface_is_pending_viewport_dst_size_int(surface)) {
//		assert(surface->viewport_resource);
//
//		wl_resource_post_error(surface->viewport_resource,
//			WP_VIEWPORT_ERROR_BAD_SIZE,
//			"wl_surface@%d viewport dst size not integer",
//			wl_resource_get_id(resource));
//		return;
//	}
//
//	if (sub) {
//		weston_subsurface_commit(sub);
//		return;
//	}
//
//	weston_surface_commit(surface);
//
//	wl_list_for_each(sub, &surface->subsurface_list, parent_link) {
//		if (sub->surface != surface)
//			weston_subsurface_parent_commit(sub, 0);
//	}

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

	if(subsurface) {
		subsurface->commit();
	}

	if(pending.buffer)
		pending.buffer->incr_use_count();
	if(buffer)
		buffer->decr_use_count();
	buffer = pending.buffer;
	pending.buffer = nullptr;

	if(buffer) {
		auto texture = buffer->ensure_texture();
		meta_surface_actor_wayland_set_texture(META_SURFACE_ACTOR_WAYLAND(actor), texture);

		if(pending.damage_surface)
			buffer->process_damage(pending.damage_surface);

	}

	/* clear damages */
	if(pending.damage_surface) {
		cairo_region_destroy(pending.damage_surface);
		pending.damage_surface = nullptr;
	}

	if(input_region != pending.input_region) {
		cairo_region_destroy(input_region);
		input_region = pending.input_region;
		cairo_region_reference(input_region);
	}

	if(opaque_region != pending.opaque_region) {
		cairo_region_destroy(opaque_region);
		opaque_region = pending.opaque_region;
		cairo_region_reference(opaque_region);
	}

	frame_callback_list.splice(frame_callback_list.end(), frame_callback_list,
			pending.frame_callback_list.begin(), pending.frame_callback_list.end());

	/* this sync the current surface state to the actor */
	meta_surface_actor_wayland_sync_state (META_SURFACE_ACTOR_WAYLAND(actor));

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

	pending.buffer_viewport.buffer.transform = transform;
	pending.buffer_viewport.changed = 1;
}

void wl_surface::recv_set_buffer_scale(struct wl_client * client, struct wl_resource * resource, int32_t scale) {

	if (scale < 1) {
		wl_resource_post_error(resource,
			WL_SURFACE_ERROR_INVALID_SCALE,
			"buffer scale must be at least one "
			"('%d' specified)", scale);
		return;
	}

	pending.buffer_viewport.buffer.scale = scale;
	pending.buffer_viewport.changed = 1;

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
