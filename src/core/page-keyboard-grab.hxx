/*
 * Copyright (2016) Benoit Gschwind
 *
 * page-keyboard-grab.hxx is part of page-compositor.
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

#ifndef PAGE_KEYBOARD_GRAB_HXX_
#define PAGE_KEYBOARD_GRAB_HXX_

#include <clutter/clutter.h>

namespace page {

struct page_keyboard_grab {

	page_keyboard_grab();
	virtual ~page_keyboard_grab();

	virtual bool key(ClutterEvent const & event) = 0;
	virtual void modifiers() = 0;
	virtual void cancel() = 0;

};

} /* namespace page */

#endif /* PAGE_KEYBOARD_GRAB_HXX_ */
