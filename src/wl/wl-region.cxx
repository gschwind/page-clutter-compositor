/*
 * Copyright (2016) Benoit Gschwind
 *
 * wl-region.cxx is part of page-compositor.
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

#include "wl-region.hxx"

#include "libpage/utils.hxx"

namespace page {
namespace wl {

wl_region::wl_region(struct wl_client *client, uint32_t version, uint32_t id) :
	wl_region_vtable{client, version, id},
	_region{nullptr}
{
	_region = cairo_region_create();
}

wl_region::~wl_region() {
	cairo_region_destroy(_region);
}

wl_region * wl_region::get(struct wl_resource * r) {
	return dynamic_cast<wl_region*>(resource_get<wl_region_vtable>(r));
}

void wl_region::recv_destroy(struct wl_client * client, struct wl_resource * resource) {
	wl_resource_destroy(_self_resource);
}

void wl_region::recv_add(struct wl_client * client, struct wl_resource * resource, int32_t x, int32_t y, int32_t width, int32_t height) {
	cairo_rectangle_int_t rect = {x, y, width, height};
	cairo_region_union_rectangle(_region, &rect);
}

void wl_region::recv_subtract(struct wl_client * client, struct wl_resource * resource, int32_t x, int32_t y, int32_t width, int32_t height) {
	cairo_rectangle_int_t rect = {x, y, width, height};
	cairo_region_subtract_rectangle(_region, &rect);
}

void wl_region::delete_resource(struct wl_resource * resource) {
	delete this;
}

}
} /* namespace page */
