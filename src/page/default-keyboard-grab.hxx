/*
 * Copyright (2017) Benoit Gschwind
 *
 * default-keyboard-grab.hxx is part of page-compositor.
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

#ifndef SRC_PAGE_DEFAULT_KEYBOARD_GRAB_HXX_
#define SRC_PAGE_DEFAULT_KEYBOARD_GRAB_HXX_

#include "libpage/page-default-keyboard-grab.hxx"

namespace page {

struct default_keyboard_grab : public page_default_keyboard_grab {
	page_context_t * _ctx;

};

}



#endif /* SRC_PAGE_DEFAULT_KEYBOARD_GRAB_HXX_ */
