/*
 * Copyright (2016) Benoit Gschwind
 *
 * wl-shell-surface.hxx is part of page-compositor.
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

#ifndef WL_WL_SHELL_SURFACE_HXX_
#define WL_WL_SHELL_SURFACE_HXX_

#include "wl-types.hxx"
#include "page-types.hxx"
#include "wayland-interface.hxx"

namespace page {
namespace wl {

using namespace wayland_cxx_wrapper;

struct wl_shell_surface : public wl_shell_surface_vtable {
	page_core * core;
	wl_surface * surface;

	wl_shell_surface(struct wl_client *client, uint32_t version, uint32_t id, page_core * core, wl_surface * surface);
	virtual ~wl_shell_surface();

	/* wl_shell_surface_vtable */
	virtual void recv_pong(struct wl_client * client, struct wl_resource * resource, uint32_t serial) override;
	virtual void recv_move(struct wl_client * client, struct wl_resource * resource, struct wl_resource * seat, uint32_t serial) override;
	virtual void recv_resize(struct wl_client * client, struct wl_resource * resource, struct wl_resource * seat, uint32_t serial, uint32_t edges) override;
	virtual void recv_set_toplevel(struct wl_client * client, struct wl_resource * resource) override;
	virtual void recv_set_transient(struct wl_client * client, struct wl_resource * resource, struct wl_resource * parent, int32_t x, int32_t y, uint32_t flags) override;
	virtual void recv_set_fullscreen(struct wl_client * client, struct wl_resource * resource, uint32_t method, uint32_t framerate, struct wl_resource * output) override;
	virtual void recv_set_popup(struct wl_client * client, struct wl_resource * resource, struct wl_resource * seat, uint32_t serial, struct wl_resource * parent, int32_t x, int32_t y, uint32_t flags) override;
	virtual void recv_set_maximized(struct wl_client * client, struct wl_resource * resource, struct wl_resource * output) override;
	virtual void recv_set_title(struct wl_client * client, struct wl_resource * resource, char const * title) override;
	virtual void recv_set_class(struct wl_client * client, struct wl_resource * resource, char const * class_) override;
	virtual void delete_resource(struct wl_resource * resource) override;

};

}
} /* namespace page */

#endif /* WL_WL_SHELL_SURFACE_HXX_ */
