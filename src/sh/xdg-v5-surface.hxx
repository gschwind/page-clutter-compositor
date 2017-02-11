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

#include <map>

#include "../page/ui-types.hxx"
#include "xdg-shell-unstable-v5-interface.hxx"

#include "wl/wl-types.hxx"
#include "sh/sh-types.hxx"


#include "utils/signals.hxx"
#include "page/page.hxx"
#include "page/surface.hxx"


namespace page {
namespace sh {

using namespace std;
using namespace wayland_cxx_wrapper;

struct xdg_v5_surface : public xdg_surface_vtable, public surface_t {
	xdg_v5_shell * shell;

	page_t *       _ctx;
	wl_client *            _client;
	wl::wl_surface *       _surface;
	uint32_t               _id;
	struct wl_resource *   _resource;
	wl_listener            _surface_destroy;

	friend class page::page_t;

	struct _state {
		std::string title;
		bool fullscreen;
		bool maximized;
		bool minimized;
		surface_t * transient_for;
		rect geometry;

		_state() {
			fullscreen = false;
			maximized = false;
			minimized = false;
			title = "";
			transient_for = nullptr;
			geometry = rect{0,0,0,0};
		}

	} _pending, _current;

	/* 0 if ack by client, otherwise the last serial sent */
	uint32_t _ack_serial;

	signal<xdg_v5_surface *> destroy;

	static map<uint32_t, edge_e> const _edge_map;

	/* avoid copy */
	xdg_v5_surface(xdg_v5_surface const &) = delete;
	xdg_v5_surface & operator=(xdg_v5_surface const &) = delete;

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

	/* page_surface_interface */
	virtual wl::wl_surface * surface() const override;
	virtual int32_t width() const override;
	virtual int32_t height() const override;
	virtual string const & title() const override;
	virtual void send_configure(int32_t width, int32_t height, set<uint32_t> const & states) override;
	virtual void send_close() override;
	virtual void send_configure_popup(int32_t x, int32_t y, int32_t width, int32_t height) override;


};

}
} /* namespace page */

#endif /* SH_XDG_V5_SURFACE_HXX_ */
