/*
 * Copyright (2016) Benoit Gschwind
 *
 * page-default-keyboard-grab.cxx is part of page-compositor.
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

#include "page-default-keyboard-grab.hxx"

#include "page-keyboard.hxx"

namespace page {

page_default_keyboard_grab::page_default_keyboard_grab(page_keyboard * keyboard) :
		keyboard{keyboard}
{
	// TODO Auto-generated constructor stub

}

page_default_keyboard_grab::~page_default_keyboard_grab()
{
	// TODO Auto-generated destructor stub
}

bool page_default_keyboard_grab::key(ClutterEvent const & event)
{
	  gboolean is_press = event.type == CLUTTER_KEY_PRESS;
	  guint32 code;

	  /* Synthetic key events are for autorepeat. Ignore those, as
	   * autorepeat in Wayland is done on the client side. */
	  if (event.key.flags & CLUTTER_EVENT_FLAG_SYNTHETIC)
	    return false;

	  /* x11 use keycode + 8 */
	  code = event.key.hardware_keycode - 8;
	  return keyboard->broadcast_key(event.key.time, code, is_press);
}

void page_default_keyboard_grab::modifiers()
{
	keyboard->broadcast_modifiers();
}

void page_default_keyboard_grab::cancel()
{

}

} /* namespace page */
