/*
 * Copyright (2017) Benoit Gschwind
 *
 * default-pointer-grab.cxx is part of page-compositor.
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

#include "default-pointer-grab.hxx"

#include "core/page-pointer.hxx"
#include "core/page-keyboard.hxx"
#include "core/page-seat.hxx"

#include "page/page.hxx"
#include "page/page_root.hxx"

namespace page {

default_pointer_grab::default_pointer_grab(page_t * ctx) :
		_ctx{ctx}
{
	// TODO Auto-generated constructor stub

}

default_pointer_grab::~default_pointer_grab()
{
	// TODO Auto-generated destructor stub
}

void default_pointer_grab::focus(ClutterEvent const & event)
{
	auto pointer = _ctx->seat->pointer;
	/* when button is pressed, passive grab is active pointer grab, thus do not
	 * refocus */
	if(pointer->button_count > 0)
		return;

	auto actor = pointer->pick_actor(&event);
	if(META_IS_SURFACE_ACTOR_WAYLAND(actor)) {
		wl_fixed_t sx, sy;
		auto surface = meta_surface_actor_wayland_get_surface(META_SURFACE_ACTOR_WAYLAND(actor));
		pointer->get_relative_coordinates(*surface, sx, sx);
		pointer->set_focus(surface, sx, sy);
	} else {
		pointer->set_focus(nullptr, wl_fixed_from_int(-1), wl_fixed_from_int(-1));
	}
}

void default_pointer_grab::motion(ClutterEvent const & event)
{
	focus(event);
	_ctx->seat->pointer->broadcast_motion(event);
	if(_ctx->seat->pointer->focus_surface == nullptr)
		_ctx->_root->broadcast_motion(this, event);
}

void default_pointer_grab::button(ClutterEvent const & event)
{
	auto pointer = _ctx->seat->pointer;

	// Send event to client if required
	// refocus if needed.
	pointer->broadcast_button(event);

	// click on given surface, will focus keyboard on it, if this is not
	// already the case.
	if (event.type == CLUTTER_BUTTON_PRESS) {
		wl_fixed_t sx, sy;
		auto surface = pointer->pick_surface_actor(&event, sx, sy);
		if(surface)
			pointer->seat->keyboard->set_focus(surface);
	}

	if(_ctx->seat->pointer->focus_surface == nullptr)
		_ctx->_root->broadcast_button(this, event);

}

void default_pointer_grab::axis(ClutterEvent const & event)
{
	/* TODO */
}

void default_pointer_grab::axis_source(uint32_t source)
{
	/* TODO */
}

void default_pointer_grab::frame()
{
	_ctx->seat->pointer->broadcast_frame();
}

void default_pointer_grab::cancel()
{

}

} /* namespace page */
