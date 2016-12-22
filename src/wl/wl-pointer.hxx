/*
 * Copyright (2016) Benoit Gschwind
 *
 * pointer.hxx is part of page-compositor.
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

#ifndef SRC_POINTER_HXX_
#define SRC_POINTER_HXX_

#include "wayland-interface.hxx"

namespace page {
namespace wl {

using namespace wcxx;

struct wl_pointer : private wl_pointer_vtable {

	wl_pointer(struct wl_client *client, uint32_t version, uint32_t id);
	virtual ~wl_pointer();

	/* wl_pointer_vtable */
	virtual void recv_set_cursor(struct wl_client * client, struct wl_resource * resource, uint32_t serial, struct wl_resource * surface, int32_t hotspot_x, int32_t hotspot_y) override;
	virtual void recv_release(struct wl_client * client, struct wl_resource * resource) override;
	virtual void delete_resource(struct wl_resource * resource) override;
};

}
}

#endif /* SRC_POINTER_HXX_ */
