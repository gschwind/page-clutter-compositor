/*
 * Copyright (2016) Benoit Gschwind
 *
 * wl-subcompositor.hxx is part of page-compositor.
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

#ifndef WL_WL_SUBCOMPOSITOR_HXX_
#define WL_WL_SUBCOMPOSITOR_HXX_

#include "wayland-interface.hxx"

namespace page {
namespace wl {

using namespace wcxx;

struct wl_subcompositor : public wl_subcompositor_vtable {

	wl_subcompositor(struct wl_client *client, uint32_t version, uint32_t id);
	virtual ~wl_subcompositor();

	/* wl_subcompositor_vtable */
	virtual void recv_destroy(struct wl_client * client, struct wl_resource * resource) override;
	virtual void recv_get_subsurface(struct wl_client * client, struct wl_resource * resource, uint32_t id, struct wl_resource * surface, struct wl_resource * parent) override;
	virtual void delete_resource(struct wl_resource * resource) override;

};

}
} /* namespace page */

#endif /* WL_WL_SUBCOMPOSITOR_HXX_ */
