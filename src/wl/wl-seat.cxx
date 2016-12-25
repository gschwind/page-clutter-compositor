/*
 * Copyright (2016) Benoit Gschwind
 *
 * seat.cxx is part of page-compositor.
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

#include "wl-seat.hxx"

#include "utils.hxx"

#include "wl-pointer.hxx"
#include "wl-keyboard.hxx"
#include "wl-touch.hxx"

#include "page-seat.hxx"


namespace page {
namespace wl {

wl_seat::wl_seat(page_seat * seat, struct wl_client *client, uint32_t version, uint32_t id) :
		wl_seat_vtable{client, version, id},
		seat{seat}
{

	send_name(seat->seat_name.c_str());
	send_capabilities(seat->capabilities);

}

wl_seat::~wl_seat() {

}

wl_seat * wl_seat::get(struct wl_resource * r) {
	return dynamic_cast<wl_seat*>(resource_get<wl_seat_vtable>(r));
}

/* wl_seat_vtable */
void wl_seat::recv_get_pointer(struct wl_client * client, struct wl_resource * resource, uint32_t id) {
	//auto pointer = make_shared<wl_pointer>(client, wl_resource_get_version(resource), id);
	//pointers.push_back(pointer);

	new wl_pointer(client, wl_resource_get_version(resource), id, seat->pointer);

}

void wl_seat::recv_get_keyboard(struct wl_client * client, struct wl_resource * resource, uint32_t id) {
	//auto keyboard = make_shared<wl_keyboard>(client, wl_resource_get_version(resource), id);
	//keyboards.push_back(keyboard);

	new wl_keyboard(client, wl_resource_get_version(resource), id, seat->keyboard);

}

void wl_seat::recv_get_touch(struct wl_client * client, struct wl_resource * resource, uint32_t id) {
	//auto touch = make_shared<wl_touch>(client, wl_resource_get_version(resource), id);
	//touchs.push_back(touch);
	new wl_touch(client, wl_resource_get_version(resource), id);

}

void wl_seat::recv_release(struct wl_client * client, struct wl_resource * resource) {
	/* release is a destructor */
	wl_resource_destroy(_self_resource);
}

void wl_seat::delete_resource(struct wl_resource * resource) {
	delete this;
}

}
}


