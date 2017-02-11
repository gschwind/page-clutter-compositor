/*
 * Copyright (2016) Benoit Gschwind
 *
 * wl-surface-state.cxx is part of page-compositor.
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

#ifndef WL_WL_SURFACE_STATE_HXX_
#define WL_WL_SURFACE_STATE_HXX_

#include <wayland-server-core.h>
#include <cairo/cairo.h>
#include <list>
#include "utils/signals.hxx"
#include "wl/wl-types.hxx"

namespace page {
namespace wl {

struct wl_buffer_viewport {
	struct {
		/* wl_surface.set_buffer_transform */
		uint32_t transform;

		/* wl_surface.set_scaling_factor */
		int32_t scale;

		/*
		 * If src_width != wl_fixed_from_int(-1),
		 * then and only then src_* are used.
		 */
		wl_fixed_t src_x, src_y;
		wl_fixed_t src_width, src_height;
	} buffer;

	struct {
		/*
		 * If width == -1, the size is inferred from the buffer.
		 */
		int32_t width, height;
	} surface;

	int changed;
};

struct wl_surface_state {
//	/* wl_surface.attach */
	bool newly_attached;
	wl_buffer * buffer;
	slot buffer_destroy_listener;

	/* This define how the surface is moved regarding the current position */
	int32_t new_buffer_relative_position_x;
	int32_t new_buffer_relative_position_y;

	/* wl_surface.damage */
	cairo_region_t * damage_surface;
	/* wl_surface.damage_buffer */
	cairo_region_t * damage_buffer;

	/* wl_surface.set_opaque_region */
	bool opaque_region_set;
	cairo_region_t * opaque_region;

	/* wl_surface.set_input_region */
	bool input_region_set;
	cairo_region_t * input_region;

	/* wl_surface.frame */
	list<wl_callback*> frame_callback_list;
//
//	/* presentation.feedback */
//	struct wl_list feedback_list;
//
//	/* wl_surface.set_buffer_transform */
//	/* wl_surface.set_scaling_factor */
//	/* wp_viewport.set_source */
//	/* wp_viewport.set_destination */
//	wl_buffer_viewport buffer_viewport;

	double scale;

	wl_surface_state();
	void on_buffer_destroy(wl_buffer * buffer);
	void set_buffer(wl_buffer * new_buffer);
	void commit_from_state(wl_surface_state & state);

};

}
}

#endif /* WL_WL_SURFACE_STATE_HXX_ */
