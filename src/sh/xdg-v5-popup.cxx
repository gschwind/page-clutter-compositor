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

namespace page {
namespace sh {

xdg_v5_popup::xdg_v5_popup(struct wl_client *client, uint32_t version, uint32_t id, xdg_v5_shell * shell) :
		xdg_popup_vtable{client, version, id},
		shell{shell}
{
	// TODO Auto-generated constructor stub

}

xdg_v5_popup::~xdg_v5_popup()
{
	// TODO Auto-generated destructor stub
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

}
} /* namespace page */
