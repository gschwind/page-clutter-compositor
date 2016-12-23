/*
 * Copyright (2016) Benoit Gschwind
 *
 * page-keyboard.hxx is part of page-compositor.
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

#ifndef SRC_PAGE_KEYBOARD_HXX_
#define SRC_PAGE_KEYBOARD_HXX_

#include <list>
#include <wayland-server-core.h>

#include "page-types.hxx"
#include "wl/wl-types.hxx"

namespace page {

using namespace std;

struct page_keyboard {

	page_seat * seat;

	list<wl::wl_keyboard *> resource;

	//struct wl_list focus_resource_list;
	//struct weston_surface *focus;
	//struct wl_listener focus_resource_listener;
	uint32_t focus_serial;
	//struct wl_signal focus_signal;

//	struct weston_keyboard_grab *grab;
//	struct weston_keyboard_grab default_grab;
//	uint32_t grab_key;
//	uint32_t grab_serial;
//	uint32_t grab_time;

//	struct wl_array keys;
//
//	struct {
//		uint32_t mods_depressed;
//		uint32_t mods_latched;
//		uint32_t mods_locked;
//		uint32_t group;
//	} modifiers;
//
//	struct weston_keyboard_grab input_method_grab;
//	struct wl_resource *input_method_resource;
//
//	struct weston_xkb_info *xkb_info;
//	struct {
//		struct xkb_state *state;
//		enum weston_led leds;
//	} xkb_state;
//	struct xkb_keymap *pending_keymap;

	page_keyboard();
	~page_keyboard();

};

} /* namespace page */

#endif /* SRC_PAGE_KEYBOARD_HXX_ */
