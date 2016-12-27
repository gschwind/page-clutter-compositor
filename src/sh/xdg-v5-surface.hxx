/*
 * Copyright (2016) Benoit Gschwind
 *
 * xdg-v5-surface.hxx is part of page-compositor.
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

#ifndef SH_XDG_V5_SURFACE_HXX_
#define SH_XDG_V5_SURFACE_HXX_

#include "xdg-shell-unstable-v5-interface.hxx"

#include "wl/wl-types.hxx"
#include "sh/sh-types.hxx"

namespace page {
namespace sh {

using namespace wayland_cxx_wrapper;

struct xdg_v5_surface : public xdg_surface_vtable {
	xdg_v5_shell * shell;
	wl::wl_surface * surface;

	xdg_v5_surface(struct wl_client *client, uint32_t version, uint32_t id, xdg_v5_shell * shell, wl::wl_surface * surface);
	virtual ~xdg_v5_surface();

	/* xdg_surface_vtable */
	virtual void recv_destroy(struct wl_client * client, struct wl_resource * resource) override;
	virtual void recv_set_parent(struct wl_client * client, struct wl_resource * resource, struct wl_resource * parent) override;
	virtual void recv_set_title(struct wl_client * client, struct wl_resource * resource, char const * title) override;
	virtual void recv_set_app_id(struct wl_client * client, struct wl_resource * resource, char const * app_id) override;
	virtual void recv_show_window_menu(struct wl_client * client, struct wl_resource * resource, struct wl_resource * seat, uint32_t serial, int32_t x, int32_t y) override;
	virtual void recv_move(struct wl_client * client, struct wl_resource * resource, struct wl_resource * seat, uint32_t serial) override;
	virtual void recv_resize(struct wl_client * client, struct wl_resource * resource, struct wl_resource * seat, uint32_t serial, uint32_t edges) override;
	virtual void recv_ack_configure(struct wl_client * client, struct wl_resource * resource, uint32_t serial) override;
	virtual void recv_set_window_geometry(struct wl_client * client, struct wl_resource * resource, int32_t x, int32_t y, int32_t width, int32_t height) override;
	virtual void recv_set_maximized(struct wl_client * client, struct wl_resource * resource) override;
	virtual void recv_unset_maximized(struct wl_client * client, struct wl_resource * resource) override;
	virtual void recv_set_fullscreen(struct wl_client * client, struct wl_resource * resource, struct wl_resource * output) override;
	virtual void recv_unset_fullscreen(struct wl_client * client, struct wl_resource * resource) override;
	virtual void recv_set_minimized(struct wl_client * client, struct wl_resource * resource) override;
	virtual void delete_resource(struct wl_resource * resource) override;

};

}
} /* namespace page */

#endif /* SH_XDG_V5_SURFACE_HXX_ */
