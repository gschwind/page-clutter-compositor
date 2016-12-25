/*
 * Copyright (2016) Benoit Gschwind
 *
 * page-default-pointer-grab.hxx is part of page-compositor.
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

#ifndef PAGE_DEFAULT_POINTER_GRAB_HXX_
#define PAGE_DEFAULT_POINTER_GRAB_HXX_

#include "page-pointer-grab.hxx"

namespace page {

struct page_default_pointer_grab : public page_pointer_grab {
	page_pointer * pointer;

	page_default_pointer_grab(page_pointer * pointer);
	virtual ~page_default_pointer_grab() override;

	virtual void focus(ClutterEvent const & event) override;
	virtual void motion(ClutterEvent * event) override;
	virtual void button(ClutterEvent * event) override;
	virtual void axis(ClutterEvent * event) override;
	virtual void axis_source(uint32_t source) override;
	virtual void frame() override;
	virtual void cancel() override;

};

} /* namespace page */

#endif /* PAGE_DEFAULT_POINTER_GRAB_HXX_ */
