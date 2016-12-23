/*
 * Copyright (2016) Benoit Gschwind
 *
 * shell.hxx is part of page-compositor.
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

#ifndef SRC_SHELL_HXX_
#define SRC_SHELL_HXX_

#include "wayland-interface.hxx"
#include "page-types.hxx"

namespace page {
namespace wl {

using namespace wcxx;

struct wl_shell : private wl_shell_vtable {
	page_core * core;

	wl_shell(struct wl_client *client, uint32_t version, uint32_t id, page_core * core);
	virtual ~wl_shell();

	/* wl_shell_vtable */
	virtual void recv_get_shell_surface(struct wl_client * client, struct wl_resource * resource, uint32_t id, struct wl_resource * surface) override;
	virtual void delete_resource(struct wl_resource * resource) override;

};

}
} /* namespace page */

#endif /* SRC_SHELL_HXX_ */
