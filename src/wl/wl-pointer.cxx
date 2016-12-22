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

namespace page {
namespace wl {

wl_pointer::wl_pointer(struct wl_client *client, uint32_t version, uint32_t id) :
		wl_pointer_vtable{client, version, id}
{

}

wl_pointer::~wl_pointer() {

}

void wl_pointer::recv_set_cursor(struct wl_client * client, struct wl_resource * resource, uint32_t serial, struct wl_resource * surface, int32_t hotspot_x, int32_t hotspot_y) {
	/* TODO */
}

void wl_pointer::recv_release(struct wl_client * client, struct wl_resource * resource) {
	/* TODO */
}

void wl_pointer::delete_resource(struct wl_resource * resource) {
	delete this;
}

}
}
