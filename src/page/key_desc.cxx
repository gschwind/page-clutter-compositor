/*
 * Copyright (2017) Benoit Gschwind
 *
 * key_desc.cxx is part of page-compositor.
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

#include "key_desc.hxx"

#include "core/page-keyboard.hxx"

namespace page {

void key_desc_t::_find_key_from_string(page_keyboard * kbd, string const & desc) {

	/* no binding is set */
	keycode = 0;
	modmask = 0;

	if(desc == "null")
		return;

	/* find all modifier */
	std::size_t bos = 0;
	std::size_t eos = desc.find(" ", bos);
	while(eos != std::string::npos) {
		std::string modifier = desc.substr(bos, eos-bos);

		/* check for supported modifier */
		if(modifier == "shift") {
			modmask |= (1 << kbd->keyboard_info.shift_mod);
		} else if (modifier == "lock") {
			modmask |= 0;
		} else if (modifier == "ctrl" or modifier == "control") {
			modmask |= (1 << kbd->keyboard_info.ctrl_mod);
		} else if (modifier == "mod1" or modifier == "alt") {
			modmask |= (1 << kbd->keyboard_info.alt_mod);
		} else if (modifier == "mod2") {
			modmask |= (1 << kbd->keyboard_info.mod2_mod);
		} else if (modifier == "mod3") {
			modmask |= (1 << kbd->keyboard_info.mod3_mod);
		} else if (modifier == "mod4" or modifier == "super") {
			modmask |= (1 << kbd->keyboard_info.super_mod);
		} else if (modifier == "mod5") {
			modmask |= (1 << kbd->keyboard_info.mod5_mod);
		} else {
			throw except("invalid modifier '%s' for key binding valid modifier are: shift, ctrl, control, alt, super, mod1, mod2, mod3, mod4 and mod5", modifier.c_str());
		}

		bos = eos+1; /* next char of char space */
		eos = desc.find(" ", bos);
	}

	keysym_name = desc.substr(bos);
	keycode = atoi(keysym_name.c_str());
	//keycode = xkb_keysym_from_name(keysym_name.c_str(), XKB_KEYSYM_NO_FLAGS);
}

}


