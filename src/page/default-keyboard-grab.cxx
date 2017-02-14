/*
 * Copyright (2017) Benoit Gschwind
 *
 * default-keyboard-grab.cxx is part of page-compositor.
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

#include "default-keyboard-grab.hxx"
#include "page/page.hxx"
#include "core/page-seat.hxx"
#include "core/page-keyboard.hxx"

namespace page {

default_keyboard_grab::default_keyboard_grab(page_t * ctx) :
		_ctx{ctx}
{

}

default_keyboard_grab::~default_keyboard_grab() {

}

bool default_keyboard_grab::key(ClutterEvent const & event)
{
	auto keyboard = _ctx->seat->keyboard;
	gboolean is_press = event.type == CLUTTER_KEY_PRESS;
	guint32 code;

	/* Synthetic key events are for autorepeat. Ignore those, as
	 * autorepeat in Wayland is done on the client side. */
	if (event.key.flags & CLUTTER_EVENT_FLAG_SYNTHETIC)
		return false;

	/* x11 use keycode + 8 */
	code = event.key.hardware_keycode - 8;

	for (auto x : _ctx->key_bindings) {
		uint32_t mod = keyboard->modifiers.mods_depressed
				| keyboard->modifiers.mods_latched;
		if (x.match(mod, code)) {
			x.handler();
			return true;
		}
	}

	return keyboard->broadcast_key(event.key.time, code, is_press);

}


void default_keyboard_grab::modifiers()
{
	auto keyboard = _ctx->seat->keyboard;
	keyboard->broadcast_modifiers();
}

void default_keyboard_grab::cancel()
{

}

}

