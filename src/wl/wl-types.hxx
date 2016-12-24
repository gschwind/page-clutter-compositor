/*
 * Copyright (2016) Benoit Gschwind
 *
 * wl-type.hxx is part of page-compositor.
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

#ifndef SRC_WL_WL_TYPES_HXX_
#define SRC_WL_WL_TYPES_HXX_

#include "config.hxx"

namespace page {
namespace wl {

struct wl_buffer;
struct wl_compositor;
struct wl_data_device;
struct wl_data_device_manager;
struct wl_keyboard;
struct wl_pointer;
struct wl_seat;
struct wl_shell;
struct wl_surface;
struct wl_touch;
struct wl_callback;
struct wl_shell_surface;
struct wl_output;
struct wl_subsurface;
struct wl_subcompositor;

}
}

#endif /* SRC_WL_WL_TYPES_HXX_ */
