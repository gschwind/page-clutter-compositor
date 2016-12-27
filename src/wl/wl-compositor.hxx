/*
 * Copyright (2016) Benoit Gschwind
 *
 * compositor.hxx is part of page-compositor.
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

#ifndef SRC_COMPOSITOR_HXX_
#define SRC_COMPOSITOR_HXX_

#include <map>

#include "wayland-interface.hxx"
#include "wl-types.hxx"
#include "page-types.hxx"


namespace page {
namespace wl {

using namespace std;
using namespace wayland_cxx_wrapper;

struct wl_compositor : private wl_compositor_vtable {
	page_core * core;

	map<struct wl_resource *, wl_buffer *> buffer_register;

	wl_compositor(struct wl_client *client, uint32_t version, uint32_t id, page_core * core);
	virtual ~wl_compositor();

	void on_buffer_destroy(struct wl_resource * r);
	wl_buffer * ensure_wl_buffer(struct wl_resource * r);

	virtual void recv_create_surface(struct wl_client * client, struct wl_resource * resource, uint32_t id) override;
	virtual void recv_create_region(struct wl_client * client, struct wl_resource * resource, uint32_t id) override;
	virtual void delete_resource(struct wl_resource * resource) override;

};

}
} /* namespace page */

#endif /* SRC_COMPOSITOR_HXX_ */
