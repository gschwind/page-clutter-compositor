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
#include <unordered_map>
#include <wayland-server-core.h>
#include <clutter/clutter.h>
#include <xkbcommon/xkbcommon.h>

#include "page-types.hxx"
#include "wl/wl-types.hxx"

namespace page {

using namespace std;

struct page_keyboard {
	page_seat * seat;

	/* store the map between clients and their pointers */
	unordered_multimap<struct wl_client *, wl::wl_keyboard *> client_keyboards;

	wl::wl_surface * focus_surface;
	uint32_t focus_serial;
	uint32_t key_serial;

	enum xkb_state_component mods_changed;

	page_keyboard_grab * grab;
	page_keyboard_grab * default_grab;

	struct {
		struct xkb_keymap *keymap;
		struct xkb_state *state;
		int keymap_fd;
		size_t keymap_size;
		char *keymap_area;

		xkb_mod_index_t shift_mod;
		xkb_mod_index_t caps_mod;
		xkb_mod_index_t ctrl_mod;
		xkb_mod_index_t alt_mod;
		xkb_mod_index_t mod2_mod;
		xkb_mod_index_t mod3_mod;
		xkb_mod_index_t super_mod;
		xkb_mod_index_t mod5_mod;

	} keyboard_info;

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

	page_keyboard(page_seat * seat);
	~page_keyboard();

	void inform_clients_of_new_keymap();
	void update_xkb_state();
	struct xkb_keymap * x11_get_keymap();
	void take_keymap(struct xkb_keymap   *keymap);

	bool handle_keyboard_event(ClutterEvent const & event);
	bool broadcast_key(uint32_t time, uint32_t key, uint32_t state);
	void broadcast_modifiers();

	void broadcast_focus();
	void set_focus (wl::wl_surface * surface);

	void register_keyboard(wl::wl_keyboard * keyboard);
	void unregister_keyboard(wl::wl_keyboard * keyboard);


};

} /* namespace page */

#endif /* SRC_PAGE_KEYBOARD_HXX_ */
