/*
 * Copyright (2016) Benoit Gschwind
 *
 * page-output.cxx is part of page-compositor.
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

#include "page-output.hxx"

#include "wayland-interface.hxx"
#include "page-core.hxx"

#include "wl/wl-output.hxx"

namespace page {

static void wrapper_bind_wl_output(struct wl_client *client, void *data, uint32_t version, uint32_t id) {
	reinterpret_cast<page_output*>(data)->bind_wl_output(client, version, id);
}

page_output::page_output(page_core * core) :
		core{core}
{
	global = wl_global_create(core->dpy, &wl_output_interface, wcxx::wl_output_vtable::INTERFACE_VERSION, this, &wrapper_bind_wl_output);


	/* TODO: create a real mode */
	page_output_mode default_mode = {
			WL_OUTPUT_MODE_CURRENT|WL_OUTPUT_MODE_PREFERRED, 1600, 1600, 60
	};

	mode_list.push_front(default_mode);
	current_mode = &mode_list.front();

}

page_output::~page_output()
{
	// TODO Auto-generated destructor stub
}

void page_output::bind_wl_output(struct wl_client *client, uint32_t version, uint32_t id)
{
	new wl::wl_output(this, client, version, id);
}

} /* namespace page */
