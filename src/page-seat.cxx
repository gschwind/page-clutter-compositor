/*
 * Copyright (2016) Benoit Gschwind
 *
 * page-seat.cxx is part of page-compositor.
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

#include "page-seat.hxx"

#include <wayland-server-core.h>
#include <wayland-server-protocol.h>

#include "page-pointer.hxx"
#include "page-keyboard.hxx"
#include "page-touch.hxx"

#include "wl/wl-seat.hxx"

namespace page {

page_seat::page_seat(uint32_t capabilities) :
		capabilities{capabilities},
		seat_name{"default"},
		pointer{nullptr},
		keyboard{nullptr},
		touch{nullptr},
		global{nullptr}
{

	if(capabilities&WL_SEAT_CAPABILITY_POINTER) {
		pointer = new page_pointer();
	}

	if(capabilities&WL_SEAT_CAPABILITY_KEYBOARD) {
		keyboard = new page_keyboard();
	}

	if(capabilities&WL_SEAT_CAPABILITY_TOUCH) {
		touch = new page_touch();
	}


}

page_seat::~page_seat() {
	// TODO Auto-generated destructor stub
}

} /* namespace page */
