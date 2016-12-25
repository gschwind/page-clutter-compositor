/*
 * Copyright (2016) Benoit Gschwind
 *
 * page-pointer-grab.hxx is part of page-compositor.
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

#ifndef PAGE_POINTER_GRAB_HXX_
#define PAGE_POINTER_GRAB_HXX_

#include <clutter/clutter.h>

#include "page-types.hxx"

namespace page {

struct page_pointer_grab {

	page_pointer_grab();
	virtual ~page_pointer_grab();

	virtual void focus(ClutterEvent const & event) = 0;
	virtual void motion(ClutterEvent * event) = 0;
	virtual void button(ClutterEvent * event) = 0;
	virtual void axis(ClutterEvent * event) = 0;
	virtual void axis_source(uint32_t source) = 0;
	virtual void frame() = 0;
	virtual void cancel() = 0;

};

} /* namespace page */

#endif /* PAGE_POINTER_GRAB_HXX_ */
