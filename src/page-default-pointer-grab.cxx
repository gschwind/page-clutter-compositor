/*
 * Copyright (2016) Benoit Gschwind
 *
 * page-default-pointer-grab.cxx is part of page-compositor.
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

#include "page-default-pointer-grab.hxx"
#include "page-pointer.hxx"
#include "page-keyboard.hxx"
#include "page-seat.hxx"

namespace page {

page_default_pointer_grab::page_default_pointer_grab(page_pointer * pointer) :
		pointer{pointer}
{
	// TODO Auto-generated constructor stub

}

page_default_pointer_grab::~page_default_pointer_grab()
{
	// TODO Auto-generated destructor stub
}

void page_default_pointer_grab::focus(ClutterEvent const & event)
{
	/* passive pointer grab, do not refocus */
	if(pointer->button_count > 0)
		return;

	wl_fixed_t sx, sy;
	auto surface = pointer->pick_actor(&event, sx, sy);
	pointer->set_focus(surface, sx, sy);

}

void page_default_pointer_grab::motion(ClutterEvent const & event)
{
	pointer->broadcast_motion(event);
}

void page_default_pointer_grab::button(ClutterEvent const & event)
{
	pointer->broadcast_button(event);

	/* TODO: remove following because it must be set by the wm */
	wl_fixed_t sx, sy;
	auto surface = pointer->pick_actor(&event, sx, sy);
	if(surface)
		pointer->seat->keyboard->set_focus(surface);
}

void page_default_pointer_grab::axis(ClutterEvent const & event)
{
	/* TODO */
}

void page_default_pointer_grab::axis_source(uint32_t source)
{
	/* TODO */
}

void page_default_pointer_grab::frame()
{
	pointer->broadcast_frame();
}

void page_default_pointer_grab::cancel()
{

}

} /* namespace page */
