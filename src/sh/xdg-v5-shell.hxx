/*
 * Copyright (2016) Benoit Gschwind
 *
 * xdg-v5-shell.hxx is part of page-compositor.
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

#ifndef SH_XDG_V5_SHELL_HXX_
#define SH_XDG_V5_SHELL_HXX_

#include "xdg-shell-unstable-v5-interface.hxx"
#include "page-types.hxx"

namespace page {
namespace sh {

using namespace wayland_cxx_wrapper;

struct xdg_v5_shell : public xdg_shell_vtable {
	page_core * core;

	page_context_t * _ctx;

	wl_client * client;

	/* resource created for xdg_shell */
	wl_resource * xdg_shell_resource;

	signal<xdg_shell_client_t *> destroy;

	map<uint32_t, xdg_surface_toplevel_t *> xdg_surface_toplevel_map;
	map<uint32_t, xdg_surface_popup_t *> xdg_surface_popup_map;

	map<struct weston_surface *, xdg_surface_base_t *> surfaces_map;

	xdg_v5_shell(struct wl_client *client, uint32_t version, uint32_t id, page_core * core);
	virtual ~xdg_v5_shell();

	/* xdg_shell_vtable */
	virtual void recv_destroy(struct wl_client * client, struct wl_resource * resource) override;
	virtual void recv_use_unstable_version(struct wl_client * client, struct wl_resource * resource, int32_t version) override;
	virtual void recv_get_xdg_surface(struct wl_client * client, struct wl_resource * resource, uint32_t id, struct wl_resource * surface) override;
	virtual void recv_get_xdg_popup(struct wl_client * client, struct wl_resource * resource, uint32_t id, struct wl_resource * surface, struct wl_resource * parent, struct wl_resource * seat, uint32_t serial, int32_t x, int32_t y) override;
	virtual void recv_pong(struct wl_client * client, struct wl_resource * resource, uint32_t serial) override;
	virtual void delete_resource(struct wl_resource * resource) override;

};

}
} /* namespace page */

#endif /* SH_XDG_V5_SHELL_HXX_ */
