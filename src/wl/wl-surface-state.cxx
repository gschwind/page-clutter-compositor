/*
 * Copyright (2016) Benoit Gschwind
 *
 * wl-surface-state.cxx is part of page-compositor.
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

#include "wl/wl-surface-state.hxx"

#include <cassert>
#include "wl/wl-buffer.hxx"

namespace page {
namespace wl {

using namespace std;

wl_surface_state::wl_surface_state() :
		newly_attached{0},
		buffer{nullptr},
		new_buffer_relative_position_x{0},
		new_buffer_relative_position_y{0},
		damage_surface{nullptr},
		damage_buffer{nullptr},
		opaque_region{nullptr},
		input_region{nullptr},
		scale{1}
{

	opaque_region = cairo_region_create();
	input_region = cairo_region_create_infini();

}

void wl_surface_state::set_buffer(wl_buffer * new_buffer) {
	if(buffer == new_buffer)
		return;
	buffer = new_buffer;
	if(new_buffer == nullptr) {
		buffer_destroy_listener.reset();
	} else {
		buffer_destroy_listener = new_buffer->destroy_signal.connect(this,
				&wl_surface_state::on_buffer_destroy);
	}
}

void wl_surface_state::on_buffer_destroy(wl_buffer * b) {
	assert(buffer == b);
	buffer = nullptr;
	buffer_destroy_listener.reset();
}

void wl_surface_state::commit_from_state(wl_surface_state & state) {

	set_buffer(state.buffer);

	if(damage_buffer != state.damage_buffer) {
		cairo_region_destroy(damage_buffer);
		damage_buffer = state.damage_buffer;
		state.damage_buffer = nullptr;
	}

	if(damage_surface != state.damage_surface) {
		cairo_region_destroy(damage_surface);
		damage_surface = state.damage_surface;
		state.damage_surface = nullptr;
	}

	/* taking in account how input region is set, we can just reference the source */
	if(input_region != state.input_region) {
		cairo_region_destroy(input_region);
		input_region = state.input_region;
		cairo_region_reference(input_region);
	}

	/* taking in account how opaque region is set, we can just reference the source */
	if(opaque_region != state.opaque_region) {
		cairo_region_destroy(opaque_region);
		opaque_region = state.opaque_region;
		cairo_region_reference(opaque_region);
	}

	new_buffer_relative_position_x = state.new_buffer_relative_position_x;
	new_buffer_relative_position_y = state.new_buffer_relative_position_y;
	frame_callback_list = state.frame_callback_list;
	scale = state.scale;

}

}
}


