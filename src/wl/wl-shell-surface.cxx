/*
 * Copyright (2016) Benoit Gschwind
 *
 * wl-shell-surface.cxx is part of page-compositor.
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

#include "wl-shell-surface.hxx"
#include "wl-surface.hxx"

#include <clutter/clutter.h>

#include "page-core.hxx"

namespace page {
namespace wl {

map<uint32_t, edge_e> const wl_shell_surface_t::_edge_map{
	{WL_SHELL_SURFACE_RESIZE_NONE, EDGE_NONE},
	{WL_SHELL_SURFACE_RESIZE_TOP, EDGE_TOP},
	{WL_SHELL_SURFACE_RESIZE_BOTTOM, EDGE_BOTTOM},
	{WL_SHELL_SURFACE_RESIZE_LEFT, EDGE_LEFT},
	{WL_SHELL_SURFACE_RESIZE_TOP_LEFT, EDGE_TOP_LEFT},
	{WL_SHELL_SURFACE_RESIZE_BOTTOM_LEFT, EDGE_BOTTOM_LEFT},
	{WL_SHELL_SURFACE_RESIZE_RIGHT, EDGE_RIGHT},
	{WL_SHELL_SURFACE_RESIZE_TOP_RIGHT, EDGE_TOP_RIGHT},
	{WL_SHELL_SURFACE_RESIZE_BOTTOM_RIGHT, EDGE_BOTTOM_RIGHT}
};


wl_shell_surface::wl_shell_surface(struct wl_client *client, uint32_t version, uint32_t id, page_core * core, wl_surface * surface) :
		wl_shell_surface_vtable{client, version, id},
		surface{surface},
		core{core},
		_ctx{ctx},
		_id{id},
		_client{client},
		_surface{surface},
		_current{},
		_ack_serial{0}
{
	_resource = wl_resource_create(_client, &wl_shell_surface_interface, 1, _id);
	wl_shell_surface_vtable::set_implementation(_resource);

	on_surface_destroy.connect(&_surface->destroy_signal, this, &wl_shell_surface_t::surface_destroyed);
	on_surface_commit.connect(&_surface->commit_signal, this, &wl_shell_surface_t::surface_commited);
}

wl_shell_surface::~wl_shell_surface() {
	// TODO Auto-generated destructor stub
	weston_log("call %s %p\n", __PRETTY_FUNCTION__, this);
	if(_surface) {
		on_surface_destroy.disconnect();
		on_surface_commit.disconnect();
	}
}

void wl_shell_surface::recv_pong(struct wl_client * client, struct wl_resource * resource, uint32_t serial) {

}

void wl_shell_surface::recv_move(struct wl_client * client, struct wl_resource * resource, struct wl_resource * seat, uint32_t serial) {
	auto seat = resource_get<struct weston_seat>(seat_resource);
	_ctx->start_move(this, seat, serial);
}

void wl_shell_surface::recv_resize(struct wl_client * client, struct wl_resource * resource, struct wl_resource * seat, uint32_t serial, uint32_t edges) {
	auto seat = resource_get<struct weston_seat>(seat_resource);
	_ctx->start_resize(this, seat, serial, edge_map(edges));
}

void wl_shell_surface::recv_set_toplevel(struct wl_client * client, struct wl_resource * resource) {
	printf("call %s (%p)\n", __PRETTY_FUNCTION__, this);

	if(clutter_actor_get_parent(CLUTTER_ACTOR(surface->actor)))
		return;

	clutter_actor_add_child(CLUTTER_ACTOR(core->_main_stage), CLUTTER_ACTOR(surface->actor));
	clutter_actor_show(CLUTTER_ACTOR(surface->actor));

	/* tell weston how to use this data */

	if(_master_view.expired()) {
		if (weston_surface_set_role(_surface, "wl_shell_surface_toplevel",
				_resource, WL_SHELL_ERROR_ROLE) < 0)
			return;
		_ctx->manage_client(this);
	}

}

void wl_shell_surface::recv_set_transient(struct wl_client * client, struct wl_resource * resource, struct wl_resource * parent, int32_t x, int32_t y, uint32_t flags) {
	_current.transient_for = parent;
}

void wl_shell_surface::recv_set_fullscreen(struct wl_client * client, struct wl_resource * resource, uint32_t method, uint32_t framerate, struct wl_resource * output) {
	_current.minimized = false;
	_current.fullscreen = true;
	_current.maximized = false;
	/* TODO: switch to fullscreen */
}

void wl_shell_surface::recv_set_popup(struct wl_client * client, struct wl_resource * resource, struct wl_resource * seat, uint32_t serial, struct wl_resource * parent, int32_t x, int32_t y, uint32_t flags) {
	if(not _master_view.expired())
		return;

	/* tell weston how to use this data */
	if (weston_surface_set_role(_surface, "wl_shell_surface_popup",
			_resource, WL_SHELL_ERROR_ROLE) < 0)
		return;

	_parent = wl_shell_surface_t::get(parent);
	_x_offset = x;
	_y_offset = y;
	_seat = resource_get<weston_seat>(seat);
	_serial = serial;

	_ctx->manage_popup(this);

	// start_popup(ps, ps, x, y)
	// start_grab_popup(ps, seat)
}

void wl_shell_surface::recv_set_maximized(struct wl_client * client, struct wl_resource * resource, struct wl_resource * output) {
	_current.minimized = false;
	_current.fullscreen = false;
	_current.maximized = true;
}

void wl_shell_surface::recv_set_title(struct wl_client * client, struct wl_resource * resource, char const * title) {
	_current.title = title;

	if(not _master_view.expired()) {
		_master_view.lock()->signal_title_change();
	}

}

void wl_shell_surface::recv_set_class(struct wl_client * client, struct wl_resource * resource, char const * class_) {
	str_class = class_;
}

void wl_shell_surface::delete_resource(struct wl_resource * resource) {
	delete this;
}


}
} /* namespace page */
