/*
 * Copyright (2016) Benoit Gschwind
 *
 * datadevicemanager.hxx is part of page-compositor.
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

#ifndef SRC_DATA_DEVICE_HXX_
#define SRC_DATA_DEVICE_HXX_

#include "wayland-interface.hxx"

#include "wl-types.hxx"

namespace page {
namespace wl {

using namespace wayland_cxx_wrapper;

struct wl_data_device : private wl_data_device_vtable {
	wl_seat * seat;

	wl_data_device(struct wl_client *client, uint32_t version, uint32_t id, wl_seat * seat);
	virtual ~wl_data_device();

	/* wl_data_device_vtable */
	virtual void recv_start_drag(struct wl_client * client, struct wl_resource * resource, struct wl_resource * source, struct wl_resource * origin, struct wl_resource * icon, uint32_t serial) override;
	virtual void recv_set_selection(struct wl_client * client, struct wl_resource * resource, struct wl_resource * source, uint32_t serial) override;
	virtual void recv_release(struct wl_client * client, struct wl_resource * resource) override;
	virtual void delete_resource(struct wl_resource * resource) override;
};

}
} /* namespace page */

#endif /* SRC_DATA_DEVICE_HXX_ */
