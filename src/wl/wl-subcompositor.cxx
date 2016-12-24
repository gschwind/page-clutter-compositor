/*
 * Copyright (2016) Benoit Gschwind
 *
 * wl-subcompositor.cxx is part of page-compositor.
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

#include "wl-subcompositor.hxx"

#include "wl/wl-subsurface.hxx"
#include "wl/wl-surface.hxx"

namespace page {
namespace wl {

wl_subcompositor::wl_subcompositor(struct wl_client *client, uint32_t version, uint32_t id) :
		wl_subcompositor_vtable{client, version, id}
{
	// TODO Auto-generated constructor stub

}

wl_subcompositor::~wl_subcompositor()
{
	// TODO Auto-generated destructor stub
}

/* wl_subcompositor_vtable */
void wl_subcompositor::recv_destroy(struct wl_client * client, struct wl_resource * resource)
{
	wl_resource_destroy(_self_resource);
}

void wl_subcompositor::recv_get_subsurface(struct wl_client * client, struct wl_resource * resource, uint32_t id, struct wl_resource * surface_resource, struct wl_resource * parent_resource)
{
	auto surface = wl_surface::get(surface_resource);
	auto parent = wl_surface::get(parent_resource);
	auto subsurface = new wl_subsurface(client, wl_resource_get_version(resource), id, surface, parent);
	parent->subsurface_pending_list.push_back(subsurface);
	surface->subsurface = subsurface;
}

void wl_subcompositor::delete_resource(struct wl_resource * resource)
{
	delete this;
}


}
} /*s namespace page */
