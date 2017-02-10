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
#include <set>
#include <functional>
#include <vector>
#include <limits>
#include <algorithm>

#include "page/box.hxx"

namespace page {

using namespace std;

template <typename T, std::size_t N>
constexpr std::size_t countof(T const (&)[N]) noexcept { return N; }

// identity is used to avoid template type deduction.
template<typename T>
struct identity {
	using type = T;
};

#define warn(test) \
	do { \
		if(not (test)) { \
			printf("WARN %s:%d (%s) fail!\n", __FILE__, __LINE__, #test); \
		} \
	} while(false)

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

template<template<typename, typename...> class C, typename T, typename ... R>
C<std::weak_ptr<T>, R...> weak(C<std::shared_ptr<T>, R...> const & x) {
	return C<std::weak_ptr<T>, R...>{x.begin(), x.end()};
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

template<typename T0, typename T1>
std::list<std::weak_ptr<T0>> filter_class(std::list<std::weak_ptr<T1>> const & x) {
	std::list<std::weak_ptr<T0>> ret;
	for (auto i : x) {
		if(i.expired())
			continue;

		auto n = dynamic_pointer_cast<T0>(i.lock());
		if (n != nullptr) {
			ret.push_back(n);
		}
	}
	return ret;
}

template<typename T0, typename T1>
std::vector<std::shared_ptr<T0>> filter_class(std::vector<std::shared_ptr<T1>> const & x) {
	std::vector<std::shared_ptr<T0>> ret;
	for (auto i : x) {
		auto n = dynamic_pointer_cast<T0>(i);
		if (n != nullptr) {
			ret.push_back(n);
		}
	}
	return ret;
}

template<typename T0, typename T1>
std::list<std::shared_ptr<T0>> filter_class(std::list<std::shared_ptr<T1>> const & x) {
	std::list<std::shared_ptr<T0>> ret;
	for (auto i : x) {
		auto n = dynamic_pointer_cast<T0>(i);
		if (n != nullptr) {
			ret.push_back(n);
		}
	}
	return ret;
}

template<template<typename, typename...> class C, typename T, typename ... R>
bool has_key(C<T, R...> const & x, T const & key) {
	auto i = std::find(x.begin(), x.end(), key);
	return i != x.end();
}

template<typename T, typename ... R>
bool has_key(std::map<T, R...> const & x, T const & key) {
	auto i = x.find(key);
	return i != x.end();
}

template<typename T>
void move_front(std::list<weak_ptr<T>> & l, shared_ptr<T> const & v) {
	auto pos = std::find_if(l.begin(), l.end(), [v](weak_ptr<T> & l) { return l.lock() == v; });
	if(pos != l.end()) {
		l.splice(l.begin(), l, pos);
	} else {
		l.push_front(v);
	}
}

template<typename T>
void move_back(std::list<weak_ptr<T>> & l, shared_ptr<T> const & v) {
	auto pos = std::find_if(l.begin(), l.end(), [v](weak_ptr<T> & l) { return l.lock() == v; });
	if(pos != l.end()) {
		l.splice(l.end(), l, pos);
	} else {
		l.push_back(v);
	}
}

template<typename T>
void move_front(std::list<T> & l, T const & v) {
	auto pos = std::find(l.begin(), l.end(), v);
	if(pos != l.end()) {
		l.splice(l.begin(), l, pos);
	} else {
		l.push_front(v);
	}
}

template<typename T>
void move_back(std::list<T> & l, T const & v) {
	auto pos = std::find(l.begin(), l.end(), v);
	if(pos != l.end()) {
		l.splice(l.end(), l, pos);
	} else {
		l.push_back(v);
	}
}


enum corner_mask_e : uint8_t {
	CAIRO_CORNER_TOPLEFT   = 0x01U,
	CAIRO_CORNER_TOPRIGHT  = 0x02U,
	CAIRO_CORNER_BOTLEFT   = 0x04U,
	CAIRO_CORNER_BOTRIGHT  = 0x08U,
	CAIRO_CORNER_ALL       = 0x0fU,
	CAIRO_CORNER_TOP       = 0x03U,
	CAIRO_CORNER_BOT       = 0x0cU,
	CAIRO_CORNER_LEFT      = 0x05U,
	CAIRO_CORNER_RIGHT     = 0x0aU
};

/**
 * Draw rectangle with all corner rounded
 **/
void cairo_rectangle_arc_corner(cairo_t * cr, double x, double y, double w, double h, double radius, uint8_t corner_mask);
void cairo_rectangle_arc_corner(cairo_t * cr, rect const & position, double radius, uint8_t corner_mask);



}

#endif /* SRC_UTILS_HXX_ */
