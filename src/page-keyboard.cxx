/*
 * Copyright (2016) Benoit Gschwind
 *
 * page-keyboard.cxx is part of page-compositor.
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

#include "page-keyboard.hxx"

#include "page-seat.hxx"

#include <algorithm>

#include "page-default-keyboard-grab.hxx"
#include "wl/wl-keyboard.hxx"
#include "wl/wl-surface.hxx"

namespace page {

page_keyboard::page_keyboard(page_seat * seat) :
		focus_surface{nullptr},
		focus_serial{0},
		key_serial{0},
		seat{seat}
{

	default_grab = new page_default_keyboard_grab{this};
	grab = default_grab;

}

page_keyboard::~page_keyboard() {
	// TODO Auto-generated destructor stub
}

bool page_keyboard::handle_keyboard_event(ClutterKeyEvent const & event)
{
	bool is_press = event.type == CLUTTER_KEY_PRESS;
	bool handled;

	/* Synthetic key events are for autorepeat. Ignore those, as
	 * autorepeat in Wayland is done on the client side. */
	if (event.flags & CLUTTER_EVENT_FLAG_SYNTHETIC)
		return false;

	//meta_verbose("Handling key %s event code %d\n",
	//		is_press ? "press" : "release", event->hardware_keycode);

//	handled = notify_key(keyboard, (const ClutterEvent *) event);

//	if (handled)
//		meta_verbose("Sent event to wayland client\n");
//	else
//		meta_verbose(
//				"No wayland surface is focused, continuing normal operation\n");

//	if (keyboard->mods_changed != 0) {
//		notify_modifiers (keyboard);
//		keyboard->mods_changed = 0;
//	}

	return handled;
}

/* This fonction eat the key events if we have a focused surface. */
bool page_keyboard::broadcast_key(uint32_t time, uint32_t key, uint32_t state)
{
	auto client = wl_resource_get_client(focus_surface->_self_resource);
	auto range = client_keyboards.equal_range(client);

	if (range.first == range.second)
		return focus_surface != nullptr;

	auto display = wl_client_get_display(client);

	key_serial = wl_display_next_serial(display);
	for (auto i = range.first; i != range.second; ++i) {
		i->second->send_key(key_serial, time, key, state);
	}

	return focus_surface != nullptr;
}

void page_keyboard::broadcast_modifiers()
{
	auto client = wl_resource_get_client(focus_surface->_self_resource);
	auto range = client_keyboards.equal_range(client);

	if (range.first == range.second)
		return;

	auto display = wl_client_get_display(client);
	uint32_t serial = wl_display_next_serial(display);

	for (auto i = range.first; i != range.second; ++i) {
		/* TODO */
		//i->second->send_modifiers(serial, time, key, state);
	}

}

void page_keyboard::register_keyboard(wl::wl_keyboard * k)
{
	auto client = wl_resource_get_client(k->_self_resource);
	client_keyboards.insert(make_pair(client, k));
}

void page_keyboard::unregister_keyboard(wl::wl_keyboard * k)
{
	auto client = wl_resource_get_client(k->_self_resource);
	auto range = client_keyboards.equal_range(client);
	auto x = find_if(range.first, range.second,
			[k](pair<struct wl_client *, wl::wl_keyboard *> x) -> bool { return k == x.second; });
	if(x != client_keyboards.end())
		client_keyboards.erase(x);
}

} /* namespace page */
