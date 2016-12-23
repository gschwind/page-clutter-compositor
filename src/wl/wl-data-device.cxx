/*
 * Copyright (2016) Benoit Gschwind
 *
 * datadevicemanager.cxx is part of page-compositor.
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

#include <wl/wl-data-device.hxx>

namespace page {
namespace wl {

wl_data_device::wl_data_device(struct wl_client *client, uint32_t version, uint32_t id) :
	wl_data_device_vtable{client, version, id}
{
	// TODO Auto-generated constructor stub

}

wl_data_device::~wl_data_device() {
	// TODO Auto-generated destructor stub
}

void wl_data_device::recv_start_drag(struct wl_client * client, struct wl_resource * resource, struct wl_resource * source, struct wl_resource * origin, struct wl_resource * icon, uint32_t serial) {

}

void wl_data_device::recv_set_selection(struct wl_client * client, struct wl_resource * resource, struct wl_resource * source, uint32_t serial) {

}

void wl_data_device::recv_release(struct wl_client * client, struct wl_resource * resource) {

}

void wl_data_device::delete_resource(struct wl_resource * resource) {
	delete this;
}

}
} /* namespace page */
