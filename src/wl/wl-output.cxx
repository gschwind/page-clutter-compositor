/*
 * Copyright (2016) Benoit Gschwind
 *
 * wl-output.cxx is part of page-compositor.
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

#include "wl-output.hxx"

#include "libpage/page-output.hxx"

namespace page {
namespace wl {

wl_output::wl_output(page_output * output, struct wl_client *client, uint32_t version, uint32_t id) :
	wl_output_vtable{client, version, id},
	output{output}
{
	send_geometry(output->x, output->y, output->mm_width, output->mm_height,
			WL_OUTPUT_SUBPIXEL_UNKNOWN, "undef manufacturer", "undef model",
			WL_OUTPUT_TRANSFORM_NORMAL);
	auto mode = output->current_mode;
	send_mode(mode->flags, mode->width, mode->height, mode->refresh);

	if(wl_resource_get_version(_self_resource) >= 2) {
		send_scale(1); /* TODO */
		send_done();
	}

}

wl_output::~wl_output()
{
	// TODO Auto-generated destructor stub
}

void wl_output::recv_release(struct wl_client * client, struct wl_resource * resource)
{
	wl_resource_destroy(_self_resource);
}

void wl_output::delete_resource(struct wl_resource * resource)
{
	delete this;
}

}
} /* namespace page */
