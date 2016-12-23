/*
 * Copyright (2016) Benoit Gschwind
 *
 * page-seat.hxx is part of page-compositor.
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

#ifndef SRC_PAGE_SEAT_HXX_
#define SRC_PAGE_SEAT_HXX_

#include <string>
#include <list>

#include <wayland-server-core.h>

#include "wl/wl-types.hxx"

namespace page {

using namespace std;

struct page_seat {

//	struct wl_list base_resource_list;
//
	struct wl_global *global;
	list<wl::wl_seat *> resources;

	//	struct weston_pointer *pointer_state;
//	struct weston_keyboard *keyboard_state;
//	struct weston_touch *touch_state;
//	int pointer_device_count;
//	int keyboard_device_count;
//	int touch_device_count;
//
//	struct weston_output *output; /* constraint */
//
//	struct wl_signal destroy_signal;
//	struct wl_signal updated_caps_signal;
//
//	struct weston_compositor *compositor;
//	struct wl_list link;
//	enum weston_keyboard_modifier modifier_state;
//	struct weston_surface *saved_kbd_focus;
//	struct wl_listener saved_kbd_focus_listener;
//	struct wl_list drag_resource_list;
//
//	uint32_t selection_serial;
//	struct weston_data_source *selection_data_source;
//	struct wl_listener selection_data_source_listener;
//	struct wl_signal selection_signal;
//
//	void (*led_update)(struct weston_seat *ws, enum weston_led leds);
//
//	struct input_method *input_method;
	string seat_name;


	page_seat();
	~page_seat();



};

} /* namespace page */

#endif /* SRC_PAGE_SEAT_HXX_ */
