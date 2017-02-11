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

#include "page-core.hxx"
#include "page-pointer.hxx"
#include "page-keyboard.hxx"
#include "page-touch.hxx"

#include "wl/wl-seat.hxx"

namespace page {


static void wrapper_bind_wl_seat(struct wl_client *client, void *data, uint32_t version, uint32_t id) {
	reinterpret_cast<page_seat*>(data)->bind_wl_seat(client, version, id);
}

page_seat::page_seat(page_core * core, uint32_t capabilities) :
		core{core},
		capabilities{capabilities},
		seat_name{"default"},
		pointer{nullptr},
		keyboard{nullptr},
		touch{nullptr},
		global{nullptr}
{

	if(capabilities&WL_SEAT_CAPABILITY_POINTER) {
		pointer = new page_pointer{this};
	}

	if(capabilities&WL_SEAT_CAPABILITY_KEYBOARD) {
		keyboard = new page_keyboard{this};
	}

	if(capabilities&WL_SEAT_CAPABILITY_TOUCH) {
		touch = new page_touch();
	}

	wl_global_create(core->dpy, &wl_seat_interface, wayland_cxx_wrapper::wl_seat_vtable::INTERFACE_VERSION, this, &wrapper_bind_wl_seat);

}

page_seat::~page_seat() {
	// TODO Auto-generated destructor stub
}

void page_seat::bind_wl_seat(struct wl_client *client, uint32_t version, uint32_t id)
{
	new wl::wl_seat(this, client, version, id);
}

} /* namespace page */
