/*
 * Copyright (2016) Benoit Gschwind
 *
 * buffer.cxx is part of page-compositor.
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

#include "wl-buffer.hxx"

#include <cogl/cogl.h>
#include <cogl/cogl-wayland-server.h>
#include <clutter/clutter.h>
#include <cassert>

namespace page {
namespace wl {

wl_buffer::wl_buffer(struct wl_resource * resource) :
	_self_resource{resource},
	texture{nullptr},
	use_count{0}
{
	// TODO Auto-generated constructor stub

}

wl_buffer::~wl_buffer() {
	destroy_signal.emit(this);
	g_clear_pointer(&texture, cogl_object_unref);
}

CoglTexture * wl_buffer::ensure_texture()
{
	  CoglContext *ctx = clutter_backend_get_cogl_context(clutter_get_default_backend ());
	  CoglError *catch_error = NULL;
	  struct wl_shm_buffer *shm_buffer;

	  g_return_val_if_fail (_self_resource, NULL);

	  if (texture)
	    return texture;

	  shm_buffer = wl_shm_buffer_get (_self_resource);

	  if (shm_buffer)
	    wl_shm_buffer_begin_access (shm_buffer);

	  texture = COGL_TEXTURE(cogl_wayland_texture_2d_new_from_buffer(ctx,
			  _self_resource, &catch_error));

	  if(shm_buffer)
	    wl_shm_buffer_end_access (shm_buffer);

	  if (not texture) {
	      cogl_error_free (catch_error);
	      printf("Could not import pending buffer, ignoring commit\n");
	  }

	  return texture;

}

void wl_buffer::process_damage(cairo_region_t * region)
{
	struct wl_shm_buffer *shm_buffer;

	shm_buffer = wl_shm_buffer_get(_self_resource);

	if (shm_buffer) {
		int i, n_rectangles;

		n_rectangles = cairo_region_num_rectangles(region);

		wl_shm_buffer_begin_access(shm_buffer);

		for (i = 0; i < n_rectangles; i++) {
			CoglError *error = NULL;
			cairo_rectangle_int_t rect;
			cairo_region_get_rectangle(region, i, &rect);
			cogl_wayland_texture_set_region_from_shm_buffer(texture,
					rect.x, rect.y, rect.width, rect.height, shm_buffer, rect.x,
					rect.y, 0, &error);

			if (error) {
				printf("Failed to set texture region: %s\n",
						error->message);
				cogl_error_free(error);
			}
		}

		wl_shm_buffer_end_access(shm_buffer);
	}
}

void wl_buffer::incr_use_count()
{
	++use_count;
}
void wl_buffer::decr_use_count()
{
	assert(use_count > 0);
	if(--use_count == 0) {
		wl_buffer_send_release(_self_resource);
	}
}

int32_t wl_buffer::width() {
	if(texture)
		return cogl_texture_get_width(texture);
	else
		return -1;
}

int32_t wl_buffer::height() {
	if(texture)
		return cogl_texture_get_height(texture);
	else
		return -1;
}

}
} /* namespace page */
