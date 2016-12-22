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

#include "utils.hxx"
#include "wayland-interface.hxx"
#include "wl-listener.hxx"

namespace page {
namespace wl {

using namespace wcxx;

/* wl_buffer never get attached to a resource, they are managed by libwayland internaly.
 * Nevertheless they are mapped to the compositor to map resource to wl_buffer.
 */
struct wl_buffer {
	struct wl_resource * _self_resource;

	signal<wl_buffer*> destroy_signal;

	/* a listener used by the compositor to know when this buffer will be destroyed */
	wl_listener_t<struct wl_resource> destroy_listener;

	wl_buffer(struct wl_resource * resource);
	~wl_buffer();

};

}
} /* namespace page */

#endif /* SRC_BUFFER_HXX_ */
