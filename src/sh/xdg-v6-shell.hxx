/*
 * Copyright (2017) Benoit Gschwind
 *
 * xdg-v6-shell.hxx is part of page-compositor.
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

#ifndef SRC_SH_XDG_V6_SHELL_HXX_
#define SRC_SH_XDG_V6_SHELL_HXX_

#include "xdg-shell-unstable-v6-interface.hxx"

#include "libpage/page-types.hxx"

namespace page {
namespace sh {

using namespace wayland_cxx_wrapper;

struct xdg_v6_shell : public zxdg_shell_v6_vtable {
	page_core * core;

	xdg_v6_shell(struct wl_client *client, uint32_t version, uint32_t id, page_core * core);
	virtual ~xdg_v6_shell();

	/* xdg_shell_vtable */
	virtual void recv_destroy(struct wl_client * client, struct wl_resource * resource) override;
	virtual void recv_create_positioner(struct wl_client * client, struct wl_resource * resource, uint32_t id) override;
	virtual void recv_get_xdg_surface(struct wl_client * client, struct wl_resource * resource, uint32_t id, struct wl_resource * surface) override;
	virtual void recv_pong(struct wl_client * client, struct wl_resource * resource, uint32_t serial) override;
	virtual void delete_resource(struct wl_resource * resource) override;

};

}
} /* namespace page */



#endif /* SRC_SH_XDG_V6_SHELL_HXX_ */
