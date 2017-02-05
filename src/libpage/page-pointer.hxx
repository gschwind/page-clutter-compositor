/*
 * Copyright (2016) Benoit Gschwind
 *
 * page-pointer.hxx is part of page-compositor.
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

#ifndef SRC_PAGE_POINTER_HXX_
#define SRC_PAGE_POINTER_HXX_

#include <list>
#include <unordered_map>
#include <wayland-server-core.h>

#include "meta/meta-surface-actor-wayland.hxx"

#include "wl/wl-types.hxx"
#include "page-types.hxx"

namespace page {

using namespace std;

struct page_pointer {
	page_seat * seat;

	ClutterInputDevice * device;

	/* store the map between clients and their pointers */
	unordered_multimap<struct wl_client *, wl::wl_pointer *> client_pointers;

	wl::wl_surface * focus_surface;
	wl_fixed_t focus_sx;
	wl_fixed_t focus_sy;
	uint32_t focus_serial;
	//struct wl_listener focus_view_listener;
	//struct wl_listener focus_resource_listener;
	//struct wl_signal focus_signal;
	//struct wl_signal motion_signal;
	//struct wl_signal destroy_signal;

	//struct weston_view *sprite;
	//struct wl_listener sprite_destroy_listener;
	//int32_t hotspot_x, hotspot_y;

	page_pointer_grab * grab;
	page_pointer_grab * default_grab;
	//wl_fixed_t grab_x, grab_y;
	//uint32_t grab_button;
	//uint32_t grab_serial;
	//uint32_t grab_time;

	//wl_fixed_t x, y;
	//wl_fixed_t sx, sy;
	uint32_t button_count;

	//struct wl_listener output_destroy_listener;

	page_pointer(page_seat * seat);
	virtual ~page_pointer();

	wl::wl_surface * pick_actor(ClutterEvent const * event, wl_fixed_t & sx, wl_fixed_t & sy);

	static int count_buttons(ClutterEvent const & event);

	void handle_pointer_event(ClutterEvent const & event);
	void update_pointer_focus_for_event(ClutterEvent const & event);
	void set_focus(wl::wl_surface * surface, wl_fixed_t sx, wl_fixed_t sy);

	void register_pointer(wl::wl_pointer * pointer);
	void unregister_pointer(wl::wl_pointer * pointer);

	void get_relative_coordinates(wl::wl_surface & surface, wl_fixed_t & sx, wl_fixed_t & sy);
	void broadcast_button(ClutterEvent const & event);
	void broadcast_motion(ClutterEvent const & event);
	void broadcast_enter(uint32_t serial, wl::wl_surface * surface);
	void broadcast_leave(uint32_t serial, wl::wl_surface * surface);
	void broadcast_frame();

};

} /* namespace page */

#endif /* SRC_PAGE_POINTER_HXX_ */
