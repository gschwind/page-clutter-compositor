/*
 * Copyright (2016) Benoit Gschwind
 *
 * wl-subsurface.cxx is part of page-compositor.
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

#include "wl-subsurface.hxx"

#include "wl/wl-surface.hxx"

namespace page {
namespace wl {

wl_subsurface::wl_subsurface(struct wl_client *client, uint32_t version, uint32_t id, wl_surface * surface, wl_surface * parent) :
		wl_subsurface_vtable{client, version, id},
		surface{surface},
		parent{parent},
		position{0, 0, false},
		synchronized{true}
{
	// TODO Auto-generated constructor stub

}

wl_subsurface::~wl_subsurface()
{
	// TODO Auto-generated destructor stub
}

auto wl_subsurface::find_sibling_in_pending_list(wl_surface * sibling) -> list<wl_subsurface*>::iterator {

	auto begin = parent->subsurface_pending_list.begin();
	auto end = parent->subsurface_pending_list.end();
	for(auto iter = begin; iter != end; iter++) {
		if((*iter)->surface == sibling)
			return iter;
	}

	return end;
}

void wl_subsurface::commit()
{
	if(position.set) {
		offset_x = position.x;
		offset_y = position.y;
		position.set = false;
	}
}

bool wl_subsurface::is_synchronized()
{
	auto subsurface = this;
	do {
		if(subsurface->synchronized)
			return true;
		subsurface = subsurface->parent->subsurface;
	} while(subsurface != nullptr);

	return false;
}

void wl_subsurface::commit_state(wl_surface_state & state)
{
	has_pending_state = true;
	synchronized_pending_state.commit_from_state(state);
}

void wl_subsurface::recv_destroy(struct wl_client * client, struct wl_resource * resource)
{
	parent->subsurface_list.remove(this);
	parent->subsurface_pending_list.remove(this);
	wl_resource_destroy(_self_resource);
}

void wl_subsurface::recv_set_position(struct wl_client * client, struct wl_resource * resource, int32_t x, int32_t y)
{
	position.x = x;
	position.y = y;
	position.set = true;
}

void wl_subsurface::recv_place_above(struct wl_client * client, struct wl_resource * resource, struct wl_resource * sibling_resource)
{
	auto sibling = wl_surface::get(sibling_resource);
	if(not sibling) {
		wl_resource_post_error(resource, WL_SUBSURFACE_ERROR_BAD_SURFACE, "sibling must be a wl_surface");
		return;
	}

	if(sibling == surface) {
		wl_resource_post_error(resource, WL_SUBSURFACE_ERROR_BAD_SURFACE, "sibling is the same subsurface");
		return;
	}

	auto cur_pos = find_sibling_in_pending_list(this->surface);
	decltype(cur_pos) new_pos;
	if(sibling == parent) {
		new_pos = parent->subsurface_pending_list.begin();
	} else {
		new_pos = find_sibling_in_pending_list(sibling);
		if(new_pos == parent->subsurface_pending_list.end()) {
			wl_resource_post_error(resource, WL_SUBSURFACE_ERROR_BAD_SURFACE, "wl_surface is not a sibling surface of the subsurface");
			return;
		}
	}

	if(cur_pos != new_pos) {
		parent->subsurface_pending_list.splice(new_pos, parent->subsurface_pending_list, cur_pos);
	}

}

void wl_subsurface::recv_place_below(struct wl_client * client, struct wl_resource * resource, struct wl_resource * sibling_resource)
{
	auto sibling = wl_surface::get(sibling_resource);
	if(not sibling) {
		wl_resource_post_error(resource, WL_SUBSURFACE_ERROR_BAD_SURFACE, "sibling must be a wl_surface");
		return;
	}

	if(sibling == surface) {
		wl_resource_post_error(resource, WL_SUBSURFACE_ERROR_BAD_SURFACE, "sibling is the same subsurface");
		return;
	}

	auto cur_pos = find_sibling_in_pending_list(this->surface);
	decltype(cur_pos) new_pos;
	if(sibling == parent) {
		new_pos = parent->subsurface_pending_list.end();
	} else {
		new_pos = find_sibling_in_pending_list(sibling);
		if(new_pos == parent->subsurface_pending_list.end()) {
			wl_resource_post_error(resource, WL_SUBSURFACE_ERROR_BAD_SURFACE, "wl_surface is not a sibling surface of the subsurface");
			return;
		}
	}

	new_pos = std::prev(new_pos);

	if(cur_pos != new_pos) {
		parent->subsurface_pending_list.splice(new_pos, parent->subsurface_pending_list, cur_pos);
	}

}

void wl_subsurface::recv_set_sync(struct wl_client * client, struct wl_resource * resource)
{
	synchronized = true;
}

void wl_subsurface::recv_set_desync(struct wl_client * client, struct wl_resource * resource)
{
	synchronized = false;
	/* TODO apply sync */
}

void wl_subsurface::delete_resource(struct wl_resource * resource)
{
	delete this;
}


}
} /* namespace page */
