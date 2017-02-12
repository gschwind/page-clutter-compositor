/*
 * Copyright (2016) Benoit Gschwind
 *
 * xdg-v5-popup.cxx is part of page-compositor.
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

#include "xdg-v5-popup.hxx"
#include "xdg-v5-shell.hxx"

#include "wl/wl-surface.hxx"
#include "sh/xdg-v5-shell.hxx"
#include "wl/wl-seat.hxx"

namespace page {
namespace sh {

xdg_v5_popup::xdg_v5_popup(struct wl_client *client, uint32_t version,
		uint32_t id,
		xdg_v5_shell * shell,
		wl::wl_surface * surface,
		wl::wl_surface * parent,
		wl::wl_seat * seat,
		uint32_t serial,
		int32_t x, int32_t y) :
		xdg_popup_vtable{client, version, id},
		shell{shell},
		_id{id},
		_client{client},
		_surface{surface}
{

	_surface->role = this;
	_parent = parent->role;
	_x_offset = x;
	_y_offset = y;

	_seat = seat->seat;
	_serial = serial;

	connect(_surface->on_commit, this, &xdg_v5_popup::surface_first_commit);
	shell->ctx->configure_popup(this);

}

xdg_v5_popup::~xdg_v5_popup()
{
	shell->ctx->destroy_surface(this); // unmanage
	destroy.emit(this);
}

void xdg_v5_popup::surface_first_commit(wl::wl_surface * es)
{
	printf("call %s\n", __PRETTY_FUNCTION__);

	shell->ctx->manage_popup(this);
	disconnect(_surface->on_commit);
	connect(_surface->on_commit, this, &xdg_v5_popup::surface_commit);

}


void xdg_v5_popup::surface_commit(wl::wl_surface * es)
{
	printf("call %s\n", __PRETTY_FUNCTION__);

}

/* xdg_popup_vtable */
void xdg_v5_popup::recv_destroy(struct wl_client * client, struct wl_resource * resource)
{
	wl_resource_destroy(_self_resource);
}

void xdg_v5_popup::delete_resource(struct wl_resource * resource)
{
	delete this;
}


/* page_surface_interface */
wl::wl_surface * xdg_v5_popup::surface() const {
	return _surface;
}

int32_t xdg_v5_popup::width() const {
	return _surface->width;
}

int32_t xdg_v5_popup::height() const {
	return _surface->height;
}

string const & xdg_v5_popup::title() const {
	static string const s{"noname"};
	return s;
}

void xdg_v5_popup::send_configure(int32_t width, int32_t height, set<uint32_t> const & states) {
	/* disabled */
}

void xdg_v5_popup::send_close() {
	/* disabled */
}

void xdg_v5_popup::send_configure_popup(int32_t x, int32_t y, int32_t width, int32_t height) {
	/* disabled */
}

}
} /* namespace page */
