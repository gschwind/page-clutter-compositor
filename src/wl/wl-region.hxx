/*
 * Copyright (2016) Benoit Gschwind
 *
 * wl-region.hxx is part of page-compositor.
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

#ifndef WL_WL_REGION_HXX_
#define WL_WL_REGION_HXX_

#include "wayland-interface.hxx"
#include "cairo/cairo.h"

namespace page {
namespace wl {

using namespace wcxx;

struct wl_region : public wl_region_vtable {
	cairo_region_t * _region;

	wl_region(struct wl_client *client, uint32_t version, uint32_t id);
	virtual ~wl_region();

	static wl_region * get(struct wl_resource * r);

	/* wl_region_vtable */
	virtual void recv_destroy(struct wl_client * client, struct wl_resource * resource) override;
	virtual void recv_add(struct wl_client * client, struct wl_resource * resource, int32_t x, int32_t y, int32_t width, int32_t height) override;
	virtual void recv_subtract(struct wl_client * client, struct wl_resource * resource, int32_t x, int32_t y, int32_t width, int32_t height) override;
	virtual void delete_resource(struct wl_resource * resource) override;

};

}
} /* namespace page */

#endif /* WL_WL_REGION_HXX_ */
