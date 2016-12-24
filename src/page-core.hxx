/*
 * Copyright (2016) Benoit Gschwind
 *
 * page.hxx is part of page-compositor.
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

#ifndef PAGE_CORE_HXX_
#define PAGE_CORE_HXX_

#include <list>
#include <wayland-server-core.h>
#include <clutter/clutter.h>

#include "config_handler.hxx"
#include "page-types.hxx"
#include "wl/wl-types.hxx"

namespace page {

struct page_core {
	static FILE * logfile;

	config_handler_t configuration;

	struct wl_display * dpy;

	struct wayland_event_source {
	  GSource source;
	  struct wl_display *display;
	} * _wayland_event_source;

	GSourceFuncs _wayland_event_source_funcs;

	ClutterActor * _main_stage;

	page_seat * seat;

	list<page_output*> output_list;

	list<wl::wl_callback *> frame_callback_queue;

	page_core();
	~page_core();

	static void log_printf(const char * fmt, va_list args);

	static gboolean wayland_event_source_prepare(GSource *base, int *timeout);
	static gboolean wayland_event_source_dispatch(GSource *base, GSourceFunc callback, void *data);

	void clutter_init(int * argc, char *** argv);
	void wayland_init();
	void wayland_event_source_init();

	static uint32_t lookup_device_capabilities(ClutterDeviceManager * device_manager);
	gboolean event_filter(ClutterEvent const * event);

	void bind_wl_compositor(struct wl_client *client, uint32_t version, uint32_t id);
	void bind_wl_data_device_manager(struct wl_client *client, uint32_t version, uint32_t id);
	void bind_wl_shell(struct wl_client *client, uint32_t version, uint32_t id);
	void bind_wl_subcompositor(struct wl_client *client, uint32_t version, uint32_t id);

	void after_stage_paint(ClutterStage * stage);
	void main_stage_destroy(ClutterActor *actor);

	void init(int * argc, char *** argv);
	void run();

};

} /* namespace page */

#endif /* PAGE_CORE_HXX_ */
