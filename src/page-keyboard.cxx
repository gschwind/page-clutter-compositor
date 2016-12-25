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

#include "wl/wl-keyboard.hxx"

namespace page {

page_keyboard::page_keyboard(page_seat * seat) :
		focus_surface{nullptr},
		focus_serial{0},
		seat{seat}
{
	// TODO Auto-generated constructor stub

}

page_keyboard::~page_keyboard() {
	// TODO Auto-generated destructor stub
}

void page_keyboard::handle_keyboard_event(ClutterEvent const & event)
{

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
