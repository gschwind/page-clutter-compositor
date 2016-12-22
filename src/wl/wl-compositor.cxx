/*
 * Copyright (2016) Benoit Gschwind
 *
 * compositor.cxx is part of page-compositor.
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

#include "wl-compositor.hxx"

#include "wl-region.hxx"

namespace page {
namespace wl {

wl_compositor::wl_compositor(struct wl_client *client, uint32_t version, uint32_t id) :
	wl_compositor_vtable{client, version, id}
{
	// TODO Auto-generated constructor stub

}

wl_compositor::~wl_compositor() {
	// TODO Auto-generated destructor stub
}

void wl_compositor::recv_create_surface(struct wl_client * client, struct wl_resource * resource, uint32_t id) {

}

void wl_compositor::recv_create_region(struct wl_client * client, struct wl_resource * resource, uint32_t id) {
	new wl_region(client, wl_resource_get_version(resource), id);
}

void wl_compositor::delete_resource(struct wl_resource * resource) {
	delete this;
}

}
} /* namespace page */
