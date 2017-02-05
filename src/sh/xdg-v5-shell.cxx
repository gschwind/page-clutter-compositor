/*
 * Copyright (2016) Benoit Gschwind
 *
 * xdg-v5-shell.cxx is part of page-compositor.
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

#include "xdg-v5-shell.hxx"

#include "page-core.hxx"
#include "sh/xdg-v5-surface.hxx"
#include "wl/wl-surface.hxx"

namespace page {
namespace sh {

xdg_v5_shell::xdg_v5_shell(struct wl_client *client, uint32_t version, uint32_t id, page_core * core) :
		xdg_shell_vtable{client, version, id},
		core{core},
		_ctx{ctx},
		xdg_shell_resource{nullptr},
		client{client}
{
	// TODO Auto-generated constructor stub

}

xdg_v5_shell::~xdg_v5_shell()
{
	// TODO Auto-generated destructor stub
}

void xdg_v5_shell::recv_destroy(struct wl_client * client, struct wl_resource * resource)
{
	if(not xdg_surface_toplevel_map.empty() or not xdg_surface_popup_map.empty()) {
		wl_resource_post_error(xdg_shell_resource, XDG_SHELL_ERROR_DEFUNCT_SURFACES, "TODO");
		return;
	}

	destroy.emit(this);
	wl_resource_destroy(_self_resource);
}

void xdg_v5_shell::recv_use_unstable_version(struct wl_client * client, struct wl_resource * resource, int32_t version)
{
	/* TODO */
}

void xdg_v5_shell::recv_get_xdg_surface(struct wl_client * client, struct wl_resource * resource, uint32_t id, struct wl_resource * surface_resource)
{

	auto surface = resource_get<weston_surface>(surface_resource);

	/* disable shared_ptr, they are managed by wl_resource */
	auto xdg_surface = new xdg_surface_toplevel_t(_ctx, client, surface, id);

	surfaces_map[surface] = xdg_surface;
	xdg_surface_toplevel_map[id] = xdg_surface;
	connect(xdg_surface->destroy, this, &xdg_shell_client_t::destroy_toplevel);

	printf("exit %s\n", __PRETTY_FUNCTION__);

	auto surface = wl::wl_surface::get(surface_resource);
	new xdg_v5_surface(client, wl_resource_get_version(resource), id, this, surface);
}

void xdg_v5_shell::recv_get_xdg_popup(struct wl_client * client, struct wl_resource * resource, uint32_t id, struct wl_resource * surface, struct wl_resource * parent, struct wl_resource * seat, uint32_t serial, int32_t x, int32_t y)
{
	printf("call %s\n", __PRETTY_FUNCTION__);
	/* In our case nullptr */
	auto surface = resource_get<weston_surface>(surface_resource);
	auto parent = resource_get<weston_surface>(parent_resource);
	auto seat = resource_get<weston_seat>(seat_resource);

	printf("p=%p, x=%d, y=%d\n", surface, x, y);

	/* disable shared_ptr for now, the resource is managed by wl_resource */
	auto xdg_popup = new xdg_surface_popup_t(_ctx, client, resource,
			id, surface, surfaces_map[parent], seat, serial, x, y);

	surfaces_map[surface] = xdg_popup;
	xdg_surface_popup_map[id] = xdg_popup;
	connect(xdg_popup->destroy, this, &xdg_shell_client_t::destroy_popup);

}

void xdg_v5_shell::recv_pong(struct wl_client * client, struct wl_resource * resource, uint32_t serial)
{
	printf("call %s\n", __PRETTY_FUNCTION__);

}

void xdg_v5_shell::delete_resource(struct wl_resource * resource)
{
	printf("call %s\n", __PRETTY_FUNCTION__);
	delete this;
}

}
} /* namespace page */
