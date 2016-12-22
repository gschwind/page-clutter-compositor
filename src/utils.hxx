/*
 * Copyright (2016) Benoit Gschwind
 *
 * utils.hxx is part of page-compositor.
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

#ifndef SRC_UTILS_HXX_
#define SRC_UTILS_HXX_

#include "wayland-server-core.h"

namespace page {

template<typename T>
T * resource_get(struct wl_resource * r) {
	return reinterpret_cast<T *>(wl_resource_get_user_data(r));
}

}

#endif /* SRC_UTILS_HXX_ */
