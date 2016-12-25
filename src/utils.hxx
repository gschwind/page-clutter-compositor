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
#include <cairo/cairo.h>

#include <memory>
#include <map>
#include <list>
#include <functional>
#include <vector>
#include <limits>

namespace page {

using namespace std;

template <typename T, std::size_t N>
constexpr std::size_t countof(T const (&)[N]) noexcept { return N; }

template<typename T>
T * resource_get(struct wl_resource * r) {
	return reinterpret_cast<T *>(wl_resource_get_user_data(r));
}


template <typename T>
bool is_expired(weak_ptr<T> & x) {
	return x.expired();
}

/**
 * /!\ also remove expired !
 **/
template<typename T0>
vector<shared_ptr<T0>> lock(list<weak_ptr<T0>> & x) {
	x.remove_if(is_expired<T0>);
	vector<shared_ptr<T0>> ret;
	for(auto i: x) ret.push_back(i.lock());
	return ret;
}

cairo_region_t * cairo_region_create_infini();

}

#endif /* SRC_UTILS_HXX_ */
