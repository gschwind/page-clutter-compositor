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
#include <wayland-server-core.h>

#include "wl/wl-types.hxx"
#include "page-types.hxx"

namespace page {

using namespace std;

struct page_pointer {

	page_seat * seat;
	list<wl::wl_pointer *> pointer_clients;

	//struct weston_view *focus;
	wl::wl_pointer *focus_client;
	uint32_t focus_serial;
	//struct wl_listener focus_view_listener;
	//struct wl_listener focus_resource_listener;
	//struct wl_signal focus_signal;
	//struct wl_signal motion_signal;
	//struct wl_signal destroy_signal;

	//struct weston_view *sprite;
	//struct wl_listener sprite_destroy_listener;
	//int32_t hotspot_x, hotspot_y;

	//struct weston_pointer_grab *grab;
	//struct weston_pointer_grab default_grab;
	//wl_fixed_t grab_x, grab_y;
	//uint32_t grab_button;
	//uint32_t grab_serial;
	//uint32_t grab_time;

	//wl_fixed_t x, y;
	//wl_fixed_t sx, sy;
	//uint32_t button_count;

	//struct wl_listener output_destroy_listener;

	page_pointer();
	virtual ~page_pointer();
};

} /* namespace page */

#endif /* SRC_PAGE_POINTER_HXX_ */
