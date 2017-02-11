/*
 * Copyright (2017) Benoit Gschwind
 *
 * default-grab-handler.hxx is part of page-compositor.
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

#ifndef SRC_PAGE_DEFAULT_POINTER_GRAB_HXX_
#define SRC_PAGE_DEFAULT_POINTER_GRAB_HXX_

#include "core/page-pointer-grab.hxx"
#include "page.hxx"

namespace page {

struct default_pointer_grab : public page_pointer_grab {
	page_t * _ctx;

	default_pointer_grab(page_t * ctx);
	virtual ~default_pointer_grab() override;

	virtual void focus(ClutterEvent const & event) override;
	virtual void motion(ClutterEvent const & event) override;
	virtual void button(ClutterEvent const & event) override;
	virtual void axis(ClutterEvent const & event) override;
	virtual void axis_source(uint32_t source) override;
	virtual void frame() override;
	virtual void cancel() override;

};

}

#endif /* SRC_PAGE_DEFAULT_POINTER_GRAB_HXX_ */
