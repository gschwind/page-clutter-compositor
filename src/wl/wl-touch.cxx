/*
 * Copyright (2016) Benoit Gschwind
 *
 * touch.cxx is part of page-compositor.
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

#include "touch.hxx"

namespace page {
namespace wl {

using namespace wcxx;

touch::touch(struct wl_client *client, uint32_t version, uint32_t id) :
		wl_touch_vtable{client, version, id}
{
	// TODO Auto-generated constructor stub

}

touch::~touch() {
	// TODO Auto-generated destructor stub
}

/* wl_touch_vtable */
void touch::recv_release(struct wl_client * client, struct wl_resource * resource) {
	/* TODO */
}

void touch::delete_resource(struct wl_resource * resource) {
	/* TODO */
}

}
} /* namespace page */