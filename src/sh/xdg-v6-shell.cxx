/*
 * Copyright (2017) Benoit Gschwind
 *
 * xdg-v6-shell.cxx is part of page-compositor.
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

#include "xdg-v6-shell.hxx"

namespace page {
namespace sh {

xdg_v6_shell::xdg_v6_shell(struct wl_client *client, uint32_t version, uint32_t id, page_core * core) :
		zxdg_shell_v6_vtable(client, version, id)
{

}

xdg_v6_shell::~xdg_v6_shell()
{

}

void xdg_v6_shell::recv_destroy(struct wl_client * client, struct wl_resource * resource)
{

}

void xdg_v6_shell::recv_create_positioner(struct wl_client * client, struct wl_resource * resource, uint32_t id)
{

}

void xdg_v6_shell::recv_get_xdg_surface(struct wl_client * client, struct wl_resource * resource, uint32_t id, struct wl_resource * surface)
{

}

void xdg_v6_shell::recv_pong(struct wl_client * client, struct wl_resource * resource, uint32_t serial)
{

}

void xdg_v6_shell::delete_resource(struct wl_resource * resource)
{

}

}
}
