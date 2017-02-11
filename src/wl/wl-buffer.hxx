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

#include <cogl/cogl.h>
#include <cairo/cairo.h>

#include "core/signals.hxx"
#include "wayland-interface.hxx"
#include "core/wl-listener.hxx"

namespace page {
namespace wl {

using namespace wayland_cxx_wrapper;

/* wl_buffer never get attached to a resource, they are managed by libwayland internaly.
 * Nevertheless they are mapped to the compositor to map resource to wl_buffer.
 */
struct wl_buffer {
	struct wl_resource * _self_resource;
	CoglTexture * texture;
	signal<wl_buffer*> destroy_signal;
	/* a listener used by the compositor to know when this buffer will be destroyed */
	wl_listener_t<struct wl_resource> destroy_listener;
	int use_count;

	wl_buffer(struct wl_resource * resource);
	~wl_buffer();

	/* wl create the CoglTexture */
	CoglTexture * ensure_texture();

	/* commit damage to the texture */
	void process_damage(cairo_region_t * region);

	/* increment/decrement use count, when the use count rech 0 a release event is sent */
	void incr_use_count();
	void decr_use_count();

	int32_t width();
	int32_t height();

};

}
} /* namespace page */

#endif /* SRC_BUFFER_HXX_ */
