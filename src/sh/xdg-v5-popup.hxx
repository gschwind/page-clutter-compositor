/*
 * Copyright (2016) Benoit Gschwind
 *
 * xdg-v5-popup.hxx is part of page-compositor.
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

#ifndef SH_XDG_V5_POPUP_HXX_
#define SH_XDG_V5_POPUP_HXX_

#include "xdg-shell-unstable-v5-interface.hxx"

#include "sh-types.hxx"

namespace page {
namespace sh {

using namespace wayland_cxx_wrapper;

struct xdg_v5_popup : public xdg_popup_vtable {
	xdg_v5_shell * shell;

	page_context_t *       _ctx;
	wl_client *            _client;
	weston_surface *       _surface;
	uint32_t               _id;
	struct wl_resource *   _resource;
	wl_listener            _surface_destroy;

	wl_listener_t<struct weston_surface> on_surface_destroy;
	wl_listener_t<struct weston_surface> on_surface_commit;

	wl_client * client;
	wl_resource * resource;
	uint32_t id;
	weston_surface * _surface;
	int32_t x;
	int32_t y;

	signal<xdg_surface_popup_t *> destroy;

	xdg_v5_popup(struct wl_client *client, uint32_t version, uint32_t id, xdg_v5_shell * shell);
	virtual ~xdg_v5_popup();

	/* xdg_popup_vtable */
	virtual void recv_destroy(struct wl_client * client, struct wl_resource * resource) override;
	virtual void delete_resource(struct wl_resource * resource) override;

	/* page_surface_interface */
	virtual weston_surface * surface() const override;
	virtual weston_view * create_weston_view() override;
	virtual int32_t width() const override;
	virtual int32_t height() const override;
	virtual string const & title() const override;
	virtual void send_configure(int32_t width, int32_t height, set<uint32_t> const & states) override;
	virtual void send_close() override;
	virtual void send_configure_popup(int32_t x, int32_t y, int32_t width, int32_t height) override;


};

}
} /* namespace page */

#endif /* SH_XDG_V5_POPUP_HXX_ */
