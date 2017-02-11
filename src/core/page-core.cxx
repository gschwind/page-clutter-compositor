/*
 * Copyright (2016) Benoit Gschwind
 *
 * page.cxx is part of page-compositor.
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

#include "page-core.hxx"

#include <cstdio>
#include <clutter/wayland/clutter-wayland-compositor.h>
#include <gdk/gdk.h>
#include <wayland-server-protocol.h>
#include "xdg-shell-unstable-v5-server-protocol.h"

#include "page-seat.hxx"
#include "page-seat.hxx"
#include "page-pointer.hxx"
#include "page-output.hxx"
#include "page-keyboard.hxx"

#include "utils/exception.hxx"
#include "wayland-interface.hxx"
#include "xdg-shell-unstable-v5-interface.hxx"

#include "wl/wl-compositor.hxx"
#include "wl/wl-data-device-manager.hxx"
#include "wl/wl-shell.hxx"
#include "wl/wl-callback.hxx"
#include "wl/wl-subcompositor.hxx"

#include "sh/xdg-v5-shell.hxx"

namespace page {

using namespace wayland_cxx_wrapper;

FILE * page_core::logfile = nullptr;

static gboolean wrapper_page_event_filter(ClutterEvent const * event, gpointer user_data)
{
	return reinterpret_cast<page_core*>(user_data)->event_filter(event);
}

static void wrapper_bind_wl_compositor(struct wl_client *client, void *data, uint32_t version, uint32_t id)
{
	reinterpret_cast<page_core*>(data)->bind_wl_compositor(client, version, id);
}

static void wrapper_bind_wl_data_device_manager(struct wl_client *client, void *data, uint32_t version, uint32_t id) {
	reinterpret_cast<page_core*>(data)->bind_wl_data_device_manager(client, version, id);
}

static void wrapper_bind_wl_shell(struct wl_client *client, void *data, uint32_t version, uint32_t id) {
	reinterpret_cast<page_core*>(data)->bind_wl_shell(client, version, id);
}

static void wrapper_bind_wl_subcompositor(struct wl_client *client, void *data, uint32_t version, uint32_t id) {
	reinterpret_cast<page_core*>(data)->bind_wl_subcompositor(client, version, id);
}

static void wrapper_main_stage_destroy(ClutterActor *actor, gpointer user_data)
{
	reinterpret_cast<page_core*>(user_data)->main_stage_destroy(actor);
}

static void wrapper_after_stage_paint(ClutterStage *stage, gpointer data)
{
	return reinterpret_cast<page_core*>(data)->after_stage_paint(stage);
}

static gboolean test_draw(ClutterCanvas *canvas, cairo_t *cr, int width,
		int height, gpointer user_data)
{
	printf("paintXXX\n");
	cairo_set_operator(cr, CAIRO_OPERATOR_SOURCE);
	//cairo_rectangle(cr, 0.0, 0.0, 50.0, 50.0);
	cairo_set_source_rgba(cr, 1.0, 0.0, 1.0, 0.5);
	cairo_paint(cr);
	return FALSE;
}

static gboolean
invalidate_clock (gpointer data_)
{
  /* invalidate the contents of the canvas */
  clutter_content_invalidate (CLUTTER_CONTENT(data_));

  /* keep the timeout source */
  return G_SOURCE_CONTINUE;
}

static
gboolean
motion_test (ClutterActor *actor,
               ClutterEvent *event,
               gpointer      user_data) {
	printf("ZZZ motion\n");
}

page_core::page_core() :
		dpy{nullptr},
		_wayland_event_source{nullptr},
		_main_stage{nullptr},
		seat{nullptr}
{

	_wayland_event_source_funcs = {
		&wayland_event_source_prepare,
		NULL,
		&wayland_event_source_dispatch,
		NULL
	};

}

page_core::~page_core()
{
	// TODO Auto-generated destructor stub
}


void page_core::log_printf(const char * fmt, va_list args)
{
	if(vfprintf(page_core::logfile, fmt, args) < 0)
		throw std::runtime_error{"could not write the log file"};
}


gboolean page_core::wayland_event_source_prepare(GSource *base, int *timeout)
{
	auto source = reinterpret_cast<wayland_event_source*>(base);
	*timeout = -1;
	wl_display_flush_clients(source->display);
	return FALSE;
}

gboolean page_core::wayland_event_source_dispatch(GSource *base, GSourceFunc callback, void *data)
{
	auto source = reinterpret_cast<wayland_event_source*>(base);
	struct wl_event_loop *loop = wl_display_get_event_loop(source->display);
	wl_event_loop_dispatch(loop, 0);
	return TRUE;
}

void page_core::clutter_init(int * argc, char *** argv)
{
	if(::clutter_init(argc, argv) != CLUTTER_INIT_SUCCESS) {
		throw except("fail to initialize clutter");
	}

	_main_stage = clutter_stage_new();
	clutter_stage_set_minimum_size(CLUTTER_STAGE(_main_stage), 1600, 1600);
	clutter_stage_set_title(CLUTTER_STAGE(_main_stage), "X11 output");
	clutter_event_add_filter(CLUTTER_STAGE(_main_stage), &wrapper_page_event_filter, NULL, this);
	clutter_actor_set_width(_main_stage, 1600);
	clutter_actor_set_height(_main_stage, 1600);
	clutter_actor_set_background_color(_main_stage, CLUTTER_COLOR_Gray);
	clutter_actor_show(CLUTTER_ACTOR(_main_stage));


	/* We use connect_after() here to accomodate code in GNOME Shell that,
	 * when benchmarking drawing performance, connects to ::after-paint
	 * and calls glFinish(). The timing information from that will be
	 * more accurate if we hold off until that completes before we signal
	 * apps to begin drawing the next frame. If there are no other
	 * connections to ::after-paint, connect() vs. connect_after() doesn't
	 * matter.
	 */
	g_signal_connect_after(CLUTTER_STAGE(_main_stage), "after-paint",
			G_CALLBACK(wrapper_after_stage_paint), this);

	clutter_stage_set_sync_delay(CLUTTER_STAGE (_main_stage), 2 /* value used by mutter */);

	g_signal_connect(_main_stage, "destroy", G_CALLBACK (wrapper_main_stage_destroy), this);

}

void page_core::wayland_init() {
	wl_display_init_shm (dpy);
	wl_global_create(dpy, &wl_compositor_interface, wl_compositor_vtable::INTERFACE_VERSION, this, &wrapper_bind_wl_compositor);
	wl_global_create(dpy, &wl_data_device_manager_interface, wl_data_device_manager_vtable::INTERFACE_VERSION, this, &wrapper_bind_wl_data_device_manager);
	wl_global_create(dpy, &wl_subcompositor_interface, wl_subcompositor_vtable::INTERFACE_VERSION, this, &wrapper_bind_wl_subcompositor);

	/* TODO: should be moved to sh */
	wl_global_create(dpy, &wl_shell_interface, wl_shell_vtable::INTERFACE_VERSION, this, &wrapper_bind_wl_shell);

	auto default_output = new page_output{this};
	output_list.push_back(default_output);

	auto device_manager = clutter_device_manager_get_default();
	uint32_t capabilities = lookup_device_capabilities(device_manager);
	seat = new page_seat{this, capabilities};

}

void page_core::wayland_event_source_init()
{
  struct wl_event_loop *loop = wl_display_get_event_loop(dpy);

  _wayland_event_source = reinterpret_cast<wayland_event_source*>(g_source_new(&_wayland_event_source_funcs, sizeof(wayland_event_source)));
  _wayland_event_source->display = dpy;
  g_source_add_unix_fd (&_wayland_event_source->source,
                        wl_event_loop_get_fd (loop),
                        static_cast<GIOCondition>(static_cast<unsigned>(G_IO_IN) | static_cast<unsigned>(G_IO_ERR)));

  /* XXX: Here we are setting the wayland event source to have a
   * slightly lower priority than the X event source, because we are
   * much more likely to get confused being told about surface changes
   * relating to X clients when we don't know what's happened to them
   * according to the X protocol.
   */
  g_source_set_priority(&_wayland_event_source->source, GDK_PRIORITY_EVENTS + 1);
  g_source_attach(&_wayland_event_source->source, NULL);

}

uint32_t page_core::lookup_device_capabilities(ClutterDeviceManager * device_manager) {

	auto devices = clutter_device_manager_peek_devices(device_manager);
	uint32_t capabilities = 0;

	for(auto i = devices; i != NULL; i = i->next) {
		auto dev = CLUTTER_INPUT_DEVICE(i->data);

		if (clutter_input_device_get_device_mode(dev) == CLUTTER_INPUT_MODE_MASTER)
			continue;

		switch(clutter_input_device_get_device_type (dev)) {
		case CLUTTER_POINTER_DEVICE:
		case CLUTTER_TOUCHPAD_DEVICE:
			printf("CLUTTER_POINTER_DEVICE (%s)\n", clutter_input_device_get_device_name(dev));
			capabilities |= WL_SEAT_CAPABILITY_POINTER;
			break;
		case CLUTTER_KEYBOARD_DEVICE:
			printf("CLUTTER_KEYBOARD_DEVICE (%s)\n", clutter_input_device_get_device_name(dev));
			capabilities |= WL_SEAT_CAPABILITY_KEYBOARD;
			break;
		case CLUTTER_TOUCHSCREEN_DEVICE:
			capabilities |= WL_SEAT_CAPABILITY_TOUCH;
			printf("CLUTTER_TOUCHSCREEN_DEVICE (%s)\n", clutter_input_device_get_device_name(dev));
			break;
		default:
			break;
		}
	}

	return capabilities;

}

gboolean page_core::event_filter(ClutterEvent const * event)
{
	switch(event->type) {
	case CLUTTER_BUTTON_PRESS:
	case CLUTTER_BUTTON_RELEASE:
	case CLUTTER_MOTION:
	case CLUTTER_SCROLL:
		seat->pointer->handle_pointer_event(*event);
		break;
	case CLUTTER_KEY_PRESS:
	case CLUTTER_KEY_RELEASE:
		seat->keyboard->handle_keyboard_event(*event);
		break;
	case CLUTTER_NOTHING:
		break;
	case CLUTTER_ENTER:
		break;
	case CLUTTER_LEAVE:
		break;
	case CLUTTER_STAGE_STATE:
		break;
	case CLUTTER_DESTROY_NOTIFY:
		break;
	case CLUTTER_CLIENT_MESSAGE:
		break;
	case CLUTTER_DELETE:
		break;
	case CLUTTER_TOUCH_BEGIN:
		break;
	case CLUTTER_TOUCH_UPDATE:
		break;
	case CLUTTER_TOUCH_END:
		break;
	case CLUTTER_TOUCH_CANCEL:
		break;
	case CLUTTER_TOUCHPAD_PINCH:
		break;
	case CLUTTER_TOUCHPAD_SWIPE:
		break;
	default:
		break;
	}

	return CLUTTER_EVENT_PROPAGATE;

}

void page_core::bind_wl_compositor(struct wl_client *client, uint32_t version, uint32_t id)
{
	new wl::wl_compositor{client, version, id, this};
}

void page_core::bind_wl_data_device_manager(struct wl_client *client, uint32_t version, uint32_t id)
{
	new wl::wl_data_device_manager{client, version, id};
}

void page_core::bind_wl_shell(struct wl_client *client, uint32_t version, uint32_t id)
{
	new wl::wl_shell{client, version, id, this};
}

void page_core::bind_wl_subcompositor(struct wl_client *client, uint32_t version, uint32_t id)
{
	new wl::wl_subcompositor{client, version, id};
}

void page_core::after_stage_paint(ClutterStage * stage) {
	//printf("call %s (%p)\n", __PRETTY_FUNCTION__, this);
	/* TODO frame call back */

	for(auto x: frame_callback_queue) {
		x->send_done(g_get_monotonic_time() / 1000);
		wl_resource_destroy(x->_self_resource);
	}

	frame_callback_queue.clear();

}

void page_core::main_stage_destroy(ClutterActor *actor)
{
	clutter_main_quit();
}

void page_core::init(int * argc, char *** argv)
{
	// load default configuration from page data directory
	configuration.merge_from_file_if_exist(std::string{DATADIR "/page/page.conf"});

	{ // load configuration located in the home directory
		auto home_directory = g_getenv("HOME");
		if(home_directory) {
			char * home_configuration_file = nullptr;
			if(asprintf(&home_configuration_file, "%s/.page.conf", home_directory) < 0)
				throw std::bad_alloc{};
			configuration.merge_from_file_if_exist(home_configuration_file);
			free(home_configuration_file);
		}
	}

	{ // setup the log file.
		auto log_file_name = configuration.get_string("default", "log_file").c_str();
		if(page_core::logfile)
			fclose(page_core::logfile);
		page_core::logfile = fopen(log_file_name, "w");
		if(not page_core::logfile)
			throw except("could not open the log file `%s'", log_file_name);
		// setup the wayland log handler.
		wl_log_set_handler_server(&page_core::log_printf);
	}


	dpy = wl_display_create();

	/* before clutter_init, give the wayland server display */
	clutter_wayland_set_compositor_display(dpy);

	clutter_set_windowing_backend("x11,*");
	clutter_init(argc, argv);

	wayland_init();

	wayland_event_source_init();

	auto display_name = wl_display_add_socket_auto(dpy);
	if (display_name == NULL)
		g_error ("Failed to create socket");

}

void page_core::run()
{
	clutter_main();
}

} /* namespace page */
