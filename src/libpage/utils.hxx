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
#include <glib.h>

#include <memory>
#include <map>
#include <list>
#include <functional>
#include <vector>
#include <limits>

#include "page/box.hxx"

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

inline void cairo_clip(cairo_t * cr, rect const & clip) {
	cairo_reset_clip(cr);
	cairo_rectangle(cr, clip.x, clip.y, clip.w, clip.h);
	::cairo_clip(cr);
}

template<typename T>
struct dimention_t {
	T width;
	T height;

	dimention_t(T w, T h) : width{w}, height{h} { }
	dimention_t(dimention_t const & d) : width{d.width}, height{d.height} { }

};

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

template<typename T0>
std::list<std::weak_ptr<T0>> weak(std::list<std::shared_ptr<T0>> const & x) {
	return std::list<std::weak_ptr<T0>>{x.begin(), x.end()};
}

template<typename T0>
std::vector<std::weak_ptr<T0>> weak(std::vector<std::shared_ptr<T0>> const & x) {
	return std::vector<std::weak_ptr<T0>>{x.begin(), x.end()};
}

static std::string xformat(char const * fmt, ...) {
	va_list l;
	va_start(l, fmt);
	int n = vsnprintf(nullptr, 0, fmt, l);
	va_end(l);
	std::string ret(n, '#');
	va_start(l, fmt);
	vsnprintf(&ret[0], n+1, fmt, l);
	va_end(l);
	return ret;
}

cairo_region_t * cairo_region_create_infini();
int create_anonymous_file(off_t size, GError **error);

}

#endif /* SRC_UTILS_HXX_ */
