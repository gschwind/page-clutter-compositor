/*
 * Copyright (2016) Benoit Gschwind
 *
 * pointer.cxx is part of page-compositor.
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

#include "wl-pointer.hxx"
#include "core/page-pointer.hxx"

namespace page {
namespace wl {

wl_pointer::wl_pointer(struct wl_client *client, uint32_t version, uint32_t id, page_pointer * core_pointer) :
		wl_pointer_vtable{client, version, id},
		core_pointer{core_pointer}
{

	core_pointer->register_pointer(this);

	//	/* We use the pointer_state directly, which means we'll
	//	 * give a wl_pointer if the seat has ever had one - even though
	//	 * the spec explicitly states that this request only takes effect
	//	 * if the seat has the pointer capability.
	//	 *
	//	 * This prevents a race between the compositor sending new
	//	 * capabilities and the client trying to use the old ones.
	//	 */
	//	struct weston_pointer *pointer = seat->pointer_state;
	//	struct wl_resource *cr;
	//	struct weston_pointer_client *pointer_client;
	//
	//	if (!pointer)
	//		return;
	//
	//        cr = wl_resource_create(client, &wl_pointer_interface,
	//				wl_resource_get_version(resource), id);
	//	if (cr == NULL) {
	//		wl_client_post_no_memory(client);
	//		return;
	//	}
	//
	//	pointer_client = weston_pointer_ensure_pointer_client(pointer, client);
	//	if (!pointer_client) {
	//		wl_client_post_no_memory(client);
	//		return;
	//	}
	//
	//	wl_list_insert(&pointer_client->pointer_resources,
	//		       wl_resource_get_link(cr));
	//	wl_resource_set_implementation(cr, &pointer_interface, pointer,
	//				       unbind_pointer_client_resource);
	//
	//	if (pointer->focus && pointer->focus->surface->resource &&
	//	    wl_resource_get_client(pointer->focus->surface->resource) == client) {
	//		wl_fixed_t sx, sy;
	//
	//		weston_view_from_global_fixed(pointer->focus,
	//					      pointer->x,
	//					      pointer->y,
	//					      &sx, &sy);
	//
	//		wl_pointer_send_enter(cr,
	//				      pointer->focus_serial,
	//				      pointer->focus->surface->resource,
	//				      sx, sy);
	//		pointer_send_frame(cr);
	//	}
}

wl_pointer::~wl_pointer() {

}

void wl_pointer::recv_set_cursor(struct wl_client * client, struct wl_resource * resource, uint32_t serial, struct wl_resource * surface, int32_t hotspot_x, int32_t hotspot_y) {
	/* TODO */
}

void wl_pointer::recv_release(struct wl_client * client, struct wl_resource * resource)
{
	wl_resource_destroy(_self_resource);
}

void wl_pointer::delete_resource(struct wl_resource * resource)
{
	core_pointer->unregister_pointer(this);
	delete this;
}

}
}
