/*
 * Copyright (2016) Benoit Gschwind
 *
 * wl-data-device-manager.cxx is part of page-compositor.
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

#include "wl/wl-data-device-manager.hxx"

#include "wl/wl-seat.hxx"
#include "wl/wl-data-device.hxx"

namespace page {
namespace wl {

wl_data_device_manager::wl_data_device_manager(struct wl_client *client, uint32_t version, uint32_t id):
		wl_data_device_manager_vtable{client, version, id}

{
	// TODO Auto-generated constructor stub

}

wl_data_device_manager::~wl_data_device_manager()
{
	// TODO Auto-generated destructor stub
}

void wl_data_device_manager::recv_create_data_source(struct wl_client * client, struct wl_resource * resource, uint32_t id)
{

}

void wl_data_device_manager::recv_get_data_device(struct wl_client * client, struct wl_resource * resource, uint32_t id, struct wl_resource * seat_resource)
{
	auto seat = wl_seat::get(seat_resource);
	new wl_data_device{client, wl_resource_get_version(_self_resource), id, seat};
}

void wl_data_device_manager::delete_resource(struct wl_resource * resource)
{

}

}
} /* namespace page */
