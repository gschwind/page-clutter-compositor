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

#include "xdg-shell-unstable-v5-server-protocol.h"

#include "xdg-v5-shell.hxx"
#include "page/page_context.hxx"
#include "core/page-core.hxx"

#include "wl/wl-surface.hxx"

namespace page {
namespace sh {

map<uint32_t, edge_e> const xdg_v5_surface::_edge_map{
	{XDG_SURFACE_RESIZE_EDGE_NONE, EDGE_NONE},
	{XDG_SURFACE_RESIZE_EDGE_TOP, EDGE_TOP},
	{XDG_SURFACE_RESIZE_EDGE_BOTTOM, EDGE_BOTTOM},
	{XDG_SURFACE_RESIZE_EDGE_LEFT, EDGE_LEFT},
	{XDG_SURFACE_RESIZE_EDGE_TOP_LEFT, EDGE_TOP_LEFT},
	{XDG_SURFACE_RESIZE_EDGE_BOTTOM_LEFT, EDGE_BOTTOM_LEFT},
	{XDG_SURFACE_RESIZE_EDGE_RIGHT, EDGE_RIGHT},
	{XDG_SURFACE_RESIZE_EDGE_TOP_RIGHT, EDGE_TOP_RIGHT},
	{XDG_SURFACE_RESIZE_EDGE_BOTTOM_RIGHT, EDGE_BOTTOM_RIGHT}
};

xdg_v5_surface::xdg_v5_surface(struct wl_client *client, uint32_t version, uint32_t id, xdg_v5_shell * shell, wl::wl_surface * surface) :
		xdg_surface_vtable{client, version, id},
		shell{shell}
{
	printf("call %s (%p)\n", __PRETTY_FUNCTION__, this);
//	clutter_actor_add_child(CLUTTER_ACTOR(shell->core->_main_stage), CLUTTER_ACTOR(surface->actor));
//	clutter_actor_show(CLUTTER_ACTOR(surface->actor));
}

xdg_v5_surface::~xdg_v5_surface()
{
	// TODO Auto-generated destructor stub
}

/* xdg_surface_vtable */
void xdg_v5_surface::recv_destroy(struct wl_client * client, struct wl_resource * resource)
{
	printf("call %s %p\n", __PRETTY_FUNCTION__, this);
//	_ctx->destroy_surface(this);
//	destroy.emit(this);
//	wl_resource_destroy(_self_resource);
}

void xdg_v5_surface::recv_set_parent(struct wl_client * client, struct wl_resource * resource, struct wl_resource * parent_resource)
{
//	if (parent_resource) {
//		auto parent = xdg_v5_surface::get(parent_resource);
//		_pending.transient_for = parent->page_surface();
//	} else {
//		_pending.transient_for = nullptr;
//	}
}

void xdg_v5_surface::recv_set_title(struct wl_client * client, struct wl_resource * resource, char const * title)
{
//	_pending.title = title;
}

void xdg_v5_surface::recv_set_app_id(struct wl_client * client, struct wl_resource * resource, char const * app_id)
{
	/* TODO */
}

void xdg_v5_surface::recv_show_window_menu(struct wl_client * client, struct wl_resource * resource, struct wl_resource * seat, uint32_t serial, int32_t x, int32_t y)
{

}

void xdg_v5_surface::recv_move(struct wl_client * client, struct wl_resource * resource, struct wl_resource * seat, uint32_t serial)
{
//	auto seat = resource_get<struct weston_seat>(seat_resource);
//	_ctx->start_move(this, seat, serial);
}

void xdg_v5_surface::recv_resize(struct wl_client * client, struct wl_resource * resource, struct wl_resource * seat, uint32_t serial, uint32_t edges)
{
//	auto seat = resource_get<struct weston_seat>(seat_resource);
//	_ctx->start_resize(this, seat, serial, edge_map(edges));
}

void xdg_v5_surface::recv_ack_configure(struct wl_client * client, struct wl_resource * resource, uint32_t serial)
{
//	if(serial == _ack_serial)
//		_ack_serial = 0;
}

void xdg_v5_surface::recv_set_window_geometry(struct wl_client * client, struct wl_resource * resource, int32_t x, int32_t y, int32_t width, int32_t height)
{
//	_pending.geometry = rect(x, y, width, height);
}

void xdg_v5_surface::recv_set_maximized(struct wl_client * client, struct wl_resource * resource)
{
//	_pending.maximized = true;
}

void xdg_v5_surface::recv_unset_maximized(struct wl_client * client, struct wl_resource * resource)
{
//	_pending.maximized = false;
}

void xdg_v5_surface::recv_set_fullscreen(struct wl_client * client, struct wl_resource * resource, struct wl_resource * output)
{
//	_pending.fullscreen = true;
}

void xdg_v5_surface::recv_unset_fullscreen(struct wl_client * client, struct wl_resource * resource)
{
//	_pending.fullscreen = false;
}

void xdg_v5_surface::recv_set_minimized(struct wl_client * client, struct wl_resource * resource)
{
//	_pending.minimized = true;
}

void xdg_v5_surface::delete_resource(struct wl_resource * resource)
{
	delete this;
}


auto xdg_v5_surface::surface() const -> wl::wl_surface * {
	return _surface;
}

auto xdg_v5_surface::width() const -> int32_t {
	return 0;
//	return _surface->width;
}

auto xdg_v5_surface::height() const -> int32_t {
	return 0;
//	return _surface->height;
}

auto xdg_v5_surface::title() const -> string const & {
	return _current.title;
}

void xdg_v5_surface::send_configure(int32_t width, int32_t height, set<uint32_t> const & states) {
//	_ack_serial = wl_display_next_serial(_ctx->dpy);
//
//	wl_array array;
//	wl_array_init(&array);
//	wl_array_add(&array, sizeof(uint32_t)*states.size());
//
//	{
//		int i = 0;
//		for(auto x: states) {
//			((uint32_t*)array.data)[i] = x;
//			++i;
//		}
//	}
//
//	::xdg_surface_send_configure(_resource, width, height, &array, _ack_serial);
//	wl_array_release(&array);
//	wl_client_flush(_client);
}

void xdg_v5_surface::send_close() {
	xdg_surface_send_close(_resource);
	wl_client_flush(_client);
}

void xdg_v5_surface::send_configure_popup(int32_t x, int32_t y, int32_t width, int32_t height) {
	/* should not be called */
}

}
} /* namespace page */
