/*
 * Copyright (2016) Benoit Gschwind
 *
 * xdg-v5-surface.cxx is part of page-compositor.
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

#include "xdg-v5-surface.hxx"

#include "xdg-v5-shell.hxx"
#include "page-core.hxx"

#include "wl/wl-surface.hxx"

namespace page {
namespace sh {

xdg_v5_surface::xdg_v5_surface(struct wl_client *client, uint32_t version, uint32_t id, xdg_v5_shell * shell, wl::wl_surface * surface) :
		xdg_surface_vtable{client, version, id},
		shell{shell},
		surface{surface}
{
	printf("call %s (%p)\n", __PRETTY_FUNCTION__, this);
	clutter_actor_add_child(CLUTTER_ACTOR(shell->core->_main_stage), CLUTTER_ACTOR(surface->actor));
	clutter_actor_show(CLUTTER_ACTOR(surface->actor));
}

xdg_v5_surface::~xdg_v5_surface()
{
	// TODO Auto-generated destructor stub
}

/* xdg_surface_vtable */
void xdg_v5_surface::recv_destroy(struct wl_client * client, struct wl_resource * resource)
{
	wl_resource_destroy(_self_resource);
}

void xdg_v5_surface::recv_set_parent(struct wl_client * client, struct wl_resource * resource, struct wl_resource * parent)
{

}

void xdg_v5_surface::recv_set_title(struct wl_client * client, struct wl_resource * resource, char const * title)
{

}

void xdg_v5_surface::recv_set_app_id(struct wl_client * client, struct wl_resource * resource, char const * app_id)
{

}

void xdg_v5_surface::recv_show_window_menu(struct wl_client * client, struct wl_resource * resource, struct wl_resource * seat, uint32_t serial, int32_t x, int32_t y)
{

}

void xdg_v5_surface::recv_move(struct wl_client * client, struct wl_resource * resource, struct wl_resource * seat, uint32_t serial)
{

}

void xdg_v5_surface::recv_resize(struct wl_client * client, struct wl_resource * resource, struct wl_resource * seat, uint32_t serial, uint32_t edges)
{

}

void xdg_v5_surface::recv_ack_configure(struct wl_client * client, struct wl_resource * resource, uint32_t serial)
{

}

void xdg_v5_surface::recv_set_window_geometry(struct wl_client * client, struct wl_resource * resource, int32_t x, int32_t y, int32_t width, int32_t height)
{

}

void xdg_v5_surface::recv_set_maximized(struct wl_client * client, struct wl_resource * resource)
{

}

void xdg_v5_surface::recv_unset_maximized(struct wl_client * client, struct wl_resource * resource)
{

}

void xdg_v5_surface::recv_set_fullscreen(struct wl_client * client, struct wl_resource * resource, struct wl_resource * output)
{

}

void xdg_v5_surface::recv_unset_fullscreen(struct wl_client * client, struct wl_resource * resource)
{

}

void xdg_v5_surface::recv_set_minimized(struct wl_client * client, struct wl_resource * resource)
{

}

void xdg_v5_surface::delete_resource(struct wl_resource * resource)
{
	delete this;
}

}
} /* namespace page */
