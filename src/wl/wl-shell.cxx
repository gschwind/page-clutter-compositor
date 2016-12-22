/*
 * Copyright (2016) Benoit Gschwind
 *
 * shell.cxx is part of page-compositor.
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

#include "wl-shell.hxx"

namespace page {
namespace wl {

wl_shell::wl_shell() {
	// TODO Auto-generated constructor stub

}

wl_shell::~wl_shell() {
	// TODO Auto-generated destructor stub
}

void wl_shell::recv_get_shell_surface(struct wl_client * client, struct wl_resource * resource, uint32_t id, struct wl_resource * surface) {

}

void wl_shell::delete_resource(struct wl_resource * resource) {

}


}
} /* namespace page */