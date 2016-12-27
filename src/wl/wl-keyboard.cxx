/*
 * Copyright (2016) Benoit Gschwind
 *
 * keyboard.cxx is part of page-compositor.
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

#include "wl-keyboard.hxx"

#include <wayland-server-protocol.h>

#include "page-keyboard.hxx"

namespace page {
namespace wl {

wl_keyboard::wl_keyboard(struct wl_client *client, uint32_t version, uint32_t id, page_keyboard * keyboard) :
		wl_keyboard_vtable{client, version, id},
		keyboard{keyboard}
{
	keyboard->register_keyboard(this);
	if (keyboard->keyboard_info.keymap_fd != -1)
		send_keymap(WL_KEYBOARD_KEYMAP_FORMAT_XKB_V1,
				keyboard->keyboard_info.keymap_fd,
				keyboard->keyboard_info.keymap_size);
	send_repeat_info(400, 400);
}

wl_keyboard::~wl_keyboard() {
	keyboard->unregister_keyboard(this);
}

/* wl_keyboard_vtable */
void wl_keyboard::recv_release(struct wl_client * client, struct wl_resource * resource) {
	wl_resource_destroy(_self_resource);
}

void wl_keyboard::delete_resource(struct wl_resource * resource) {
	delete this;
}

}
} /* namespace page */
