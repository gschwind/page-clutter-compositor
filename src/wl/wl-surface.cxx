/*
 * Copyright (2016) Benoit Gschwind
 *
 * surface.cxx is part of page-compositor.
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

#include "wl-surface.hxx"

namespace page {
namespace wl {

wl_surface::wl_surface() {
	// TODO Auto-generated constructor stub

}

wl_surface::~wl_surface() {
	// TODO Auto-generated destructor stub
}

void wl_surface::recv_destroy(struct wl_client * client, struct wl_resource * resource) {

}

void wl_surface::recv_attach(struct wl_client * client, struct wl_resource * resource, struct wl_resource * buffer, int32_t x, int32_t y) {

}

void wl_surface::recv_damage(struct wl_client * client, struct wl_resource * resource, int32_t x, int32_t y, int32_t width, int32_t height) {

}

void wl_surface::recv_frame(struct wl_client * client, struct wl_resource * resource, uint32_t callback) {

}

void wl_surface::recv_set_opaque_region(struct wl_client * client, struct wl_resource * resource, struct wl_resource * region) {

}

void wl_surface::recv_set_input_region(struct wl_client * client, struct wl_resource * resource, struct wl_resource * region) {

}

void wl_surface::recv_commit(struct wl_client * client, struct wl_resource * resource){

}

void wl_surface::recv_set_buffer_transform(struct wl_client * client, struct wl_resource * resource, int32_t transform) {

}

void wl_surface::recv_set_buffer_scale(struct wl_client * client, struct wl_resource * resource, int32_t scale) {

}

void wl_surface::recv_damage_buffer(struct wl_client * client, struct wl_resource * resource, int32_t x, int32_t y, int32_t width, int32_t height) {

}

void wl_surface::delete_resource(struct wl_resource * resource){
	delete this;
}

}
} /* namespace page */
