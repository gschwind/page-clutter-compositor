/*
 * Copyright (2016) Benoit Gschwind
 *
 * wl-shell-surface.cxx is part of page-compositor.
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

#include "wl-shell-surface.hxx"
#include "wl-surface.hxx"

#include <clutter/clutter.h>

#include "page-core.hxx"

namespace page {
namespace wl {

wl_shell_surface::wl_shell_surface(struct wl_client *client, uint32_t version, uint32_t id, page_core * core, wl_surface * surface) :
		wl_shell_surface_vtable{client, version, id},
		surface{surface},
		core{core}
{

}

wl_shell_surface::~wl_shell_surface() {
	// TODO Auto-generated destructor stub
}

void wl_shell_surface::recv_pong(struct wl_client * client, struct wl_resource * resource, uint32_t serial) {

}

void wl_shell_surface::recv_move(struct wl_client * client, struct wl_resource * resource, struct wl_resource * seat, uint32_t serial) {

}

void wl_shell_surface::recv_resize(struct wl_client * client, struct wl_resource * resource, struct wl_resource * seat, uint32_t serial, uint32_t edges) {

}

void wl_shell_surface::recv_set_toplevel(struct wl_client * client, struct wl_resource * resource) {
	printf("call %s (%p)\n", __PRETTY_FUNCTION__, this);

	if(clutter_actor_get_parent(CLUTTER_ACTOR(surface->actor)))
		return;

	clutter_actor_add_child(CLUTTER_ACTOR(core->_main_stage), CLUTTER_ACTOR(surface->actor));
	clutter_actor_show(CLUTTER_ACTOR(surface->actor));
}

void wl_shell_surface::recv_set_transient(struct wl_client * client, struct wl_resource * resource, struct wl_resource * parent, int32_t x, int32_t y, uint32_t flags) {

}

void wl_shell_surface::recv_set_fullscreen(struct wl_client * client, struct wl_resource * resource, uint32_t method, uint32_t framerate, struct wl_resource * output) {

}

void wl_shell_surface::recv_set_popup(struct wl_client * client, struct wl_resource * resource, struct wl_resource * seat, uint32_t serial, struct wl_resource * parent, int32_t x, int32_t y, uint32_t flags) {

}

void wl_shell_surface::recv_set_maximized(struct wl_client * client, struct wl_resource * resource, struct wl_resource * output) {

}

void wl_shell_surface::recv_set_title(struct wl_client * client, struct wl_resource * resource, char const * title) {

}

void wl_shell_surface::recv_set_class(struct wl_client * client, struct wl_resource * resource, char const * class_) {

}

void wl_shell_surface::delete_resource(struct wl_resource * resource) {
	delete this;
}


}
} /* namespace page */
