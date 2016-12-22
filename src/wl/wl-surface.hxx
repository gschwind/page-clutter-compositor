/*
 * Copyright (2016) Benoit Gschwind
 *
 * surface.hxx is part of page-compositor.
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

#ifndef SRC_SURFACE_HXX_
#define SRC_SURFACE_HXX_

#include "wayland-interface.hxx"

namespace page {
namespace wl {

using namespace wcxx;

struct wl_surface : private wl_surface_vtable {

	wl_surface();
	virtual ~wl_surface();

	/* wl_surface_vtable */
	virtual void recv_destroy(struct wl_client * client, struct wl_resource * resource) override;
	virtual void recv_attach(struct wl_client * client, struct wl_resource * resource, struct wl_resource * buffer, int32_t x, int32_t y) override;
	virtual void recv_damage(struct wl_client * client, struct wl_resource * resource, int32_t x, int32_t y, int32_t width, int32_t height) override;
	virtual void recv_frame(struct wl_client * client, struct wl_resource * resource, uint32_t callback) = 0;
	virtual void recv_set_opaque_region(struct wl_client * client, struct wl_resource * resource, struct wl_resource * region) override;
	virtual void recv_set_input_region(struct wl_client * client, struct wl_resource * resource, struct wl_resource * region) override;
	virtual void recv_commit(struct wl_client * client, struct wl_resource * resource) = 0;
	virtual void recv_set_buffer_transform(struct wl_client * client, struct wl_resource * resource, int32_t transform) override;
	virtual void recv_set_buffer_scale(struct wl_client * client, struct wl_resource * resource, int32_t scale) override;
	virtual void recv_damage_buffer(struct wl_client * client, struct wl_resource * resource, int32_t x, int32_t y, int32_t width, int32_t height) override;
	virtual void delete_resource(struct wl_resource * resource) override;
};

}
} /* namespace page */

#endif /* SRC_SURFACE_HXX_ */
