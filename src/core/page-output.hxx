/*
 * Copyright (2016) Benoit Gschwind
 *
 * page-output.hxx is part of page-compositor.
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

#ifndef PAGE_OUTPUT_HXX_
#define PAGE_OUTPUT_HXX_

#include <wayland-server-protocol.h>
#include <list>

#include "page-types.hxx"

namespace page {

using namespace std;

struct page_output_mode {
	uint32_t flags;
	int32_t width, height;
	uint32_t refresh;
};

struct page_output {
	page_core * core;

//	uint32_t id;
//	char *name;
//
//	void *renderer_state;
//
//	struct wl_list link;
//	struct wl_list resource_list;
	struct wl_global *global;
//	struct weston_compositor *compositor;
//
//	/** From global to output buffer coordinates. */
//	struct weston_matrix matrix;
//	/** From output buffer to global coordinates. */
//	struct weston_matrix inverse_matrix;
//
//	struct wl_list animation_list;

	/* geometry */
	int32_t x, y, width, height;
	int32_t mm_width, mm_height;
//
//	/** Output area in global coordinates, simple rect */
//	pixman_region32_t region;
//
//	pixman_region32_t previous_damage;
//	int repaint_needed;
//	int repaint_scheduled;
//	struct wl_event_source *repaint_timer;
//	struct weston_output_zoom zoom;
//	int dirty;
//	struct wl_signal frame_signal;
//	struct wl_signal destroy_signal;
//	int move_x, move_y;
//	uint32_t frame_time; /* presentation timestamp in milliseconds */
//	uint64_t msc;        /* media stream counter */
//	int disable_planes;
//	int destroying;
//	struct wl_list feedback_list;
//
//	char *make, *model, *serial_number;
//	uint32_t subpixel;
//	uint32_t transform;
//	int32_t native_scale;
//	int32_t current_scale;
//	int32_t original_scale;
//

	struct page_output_mode *native_mode;
	struct page_output_mode *current_mode;
	struct page_output_mode *original_mode;
	list<page_output_mode> mode_list;

//	void (*start_repaint_loop)(struct weston_output *output);
//	int (*repaint)(struct weston_output *output,
//			pixman_region32_t *damage);
//	void (*destroy)(struct weston_output *output);
//	void (*assign_planes)(struct weston_output *output);
//	int (*switch_mode)(struct weston_output *output, struct weston_mode *mode);
//
//	/* backlight values are on 0-255 range, where higher is brighter */
//	int32_t backlight_current;
//	void (*set_backlight)(struct weston_output *output, uint32_t value);
//	void (*set_dpms)(struct weston_output *output, enum dpms_enum level);
//
//	int connection_internal;
//	uint16_t gamma_size;
//	void (*set_gamma)(struct weston_output *output,
//			  uint16_t size,
//			  uint16_t *r,
//			  uint16_t *g,
//			  uint16_t *b);
//
//	struct weston_timeline_object timeline;
//
//	bool enabled;
//	int scale;
//
//	int (*enable)(struct weston_output *output);
//	int (*disable)(struct weston_output *output);

	page_output(page_core * core);
	virtual ~page_output();

	void bind_wl_output(struct wl_client *client, uint32_t version, uint32_t id);

};

} /* namespace page */

#endif /* PAGE_OUTPUT_HXX_ */
