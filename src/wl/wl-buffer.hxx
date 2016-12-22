/*
 * Copyright (2016) Benoit Gschwind
 *
 * buffer.hxx is part of page-compositor.
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

#ifndef SRC_BUFFER_HXX_
#define SRC_BUFFER_HXX_

#include "wayland-interface.hxx"

namespace page {
namespace wl {

using namespace wcxx;

struct wl_buffer : private wl_buffer_vtable {
	wl_buffer();
	virtual ~wl_buffer();

	/* wl_buffer_vtable */
	virtual void recv_destroy(struct wl_client * client, struct wl_resource * resource) override;
	virtual void delete_resource(struct wl_resource * resource) override;
};

}
} /* namespace page */

#endif /* SRC_BUFFER_HXX_ */
