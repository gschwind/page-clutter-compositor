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

#include "page.hxx"

#include <cstdio>
#include <clutter/wayland/clutter-wayland-compositor.h>
#include <gdk/gdk.h>
#include <wayland-server-protocol.h>

#include "exception.hxx"
#include "wayland-interface.hxx"

namespace page {

using namespace wcxx;

FILE * page::logfile = nullptr;

static gboolean wrapper_page_event_filter(ClutterEvent const * event, gpointer user_data)
{
	return reinterpret_cast<page*>(user_data)->event_filter(event);
}

static void wrapper_bind_wl_compositor(struct wl_client *client, void *data, uint32_t version, uint32_t id)
{
	reinterpret_cast<page*>(data)->bind_wl_compositor(client, version, id);
}

static void wrapper_bind_wl_seat(struct wl_client *client, void *data, uint32_t version, uint32_t id) {
	reinterpret_cast<page*>(data)->bind_wl_seat(client, version, id);
}

static void wrapper_bind_wl_data_device_manager(struct wl_client *client, void *data, uint32_t version, uint32_t id) {
	reinterpret_cast<page*>(data)->bind_wl_data_device_manager(client, version, id);
}

static void wrapper_bind_wl_output(struct wl_client *client, void *data, uint32_t version, uint32_t id) {
	reinterpret_cast<page*>(data)->bind_wl_output(client, version, id);
}

static void wrapper_bind_wl_shell(struct wl_client *client, void *data, uint32_t version, uint32_t id) {
	reinterpret_cast<page*>(data)->bind_wl_shell(client, version, id);
}

static void wrapper_main_stage_destroy(ClutterActor *actor, gpointer user_data)
{
	reinterpret_cast<page*>(user_data)->main_stage_destroy(actor);
}

page::page() :
		dpy{nullptr},
		_wayland_event_source{nullptr},
		_main_stage{nullptr}
{

	_wayland_event_source_funcs = {
		&wayland_event_source_prepare,
		NULL,
		&wayland_event_source_dispatch,
		NULL
	};

}

page::~page()
{
	// TODO Auto-generated destructor stub
}


void page::log_printf(const char * fmt, va_list args)
{
	if(vfprintf(page::logfile, fmt, args) < 0)
		throw std::runtime_error{"could not write the log file"};
}


gboolean page::wayland_event_source_prepare(GSource *base, int *timeout)
{
	auto source = reinterpret_cast<wayland_event_source*>(base);
	*timeout = -1;
	wl_display_flush_clients(source->display);
	return FALSE;
}

gboolean page::wayland_event_source_dispatch(GSource *base, GSourceFunc callback, void *data)
{
	auto source = reinterpret_cast<wayland_event_source*>(base);
	struct wl_event_loop *loop = wl_display_get_event_loop(source->display);
	wl_event_loop_dispatch(loop, 0);
	return TRUE;
}

void page::clutter_init(int * argc, char *** argv)
{
	if(::clutter_init(argc, argv) != CLUTTER_INIT_SUCCESS) {
		throw except("fail to initialize clutter");
	}

	_main_stage = clutter_stage_new();
	clutter_stage_set_minimum_size(CLUTTER_STAGE(_main_stage), 200, 200);
	clutter_stage_set_title(CLUTTER_STAGE(_main_stage), "X11 output");
	clutter_event_add_filter(CLUTTER_STAGE(_main_stage), &wrapper_page_event_filter, NULL, this);

	auto text_actor = clutter_text_new();
	clutter_text_set_text(CLUTTER_TEXT(text_actor), "Clutter test text");

	clutter_actor_add_child(CLUTTER_ACTOR(_main_stage), text_actor);

	g_signal_connect(_main_stage, "destroy", G_CALLBACK (wrapper_main_stage_destroy), this);

	clutter_actor_show(CLUTTER_ACTOR(_main_stage));

}

void page::wayland_init() {
	wl_display_init_shm (dpy);
	wl_global_create(dpy, &wl_compositor_interface, wl_compositor_vtable::INTERFACE_VERSION, this, &wrapper_bind_wl_compositor);
	wl_global_create(dpy, &wl_seat_interface, wl_seat_vtable::INTERFACE_VERSION, this, &wrapper_bind_wl_seat);
	wl_global_create(dpy, &wl_output_interface, wl_output_vtable::INTERFACE_VERSION, this, &wrapper_bind_wl_output);
	wl_global_create(dpy, &wl_data_device_manager_interface, wl_data_device_manager_vtable::INTERFACE_VERSION, this, &wrapper_bind_wl_data_device_manager);
	wl_global_create(dpy, &wl_shell_interface, wl_shell_vtable::INTERFACE_VERSION, this, &wrapper_bind_wl_shell);
}

void page::wayland_event_source_init()
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

uint32_t page::lookup_device_capabilities(ClutterDeviceManager * device_manager) {

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

gboolean page::event_filter(ClutterEvent const * event)
{
	switch(event->type) {
	case CLUTTER_NOTHING:
		break;
	case CLUTTER_KEY_PRESS:
		printf("KeyPress (%d)\n", clutter_event_get_key_code(event));
		break;
	case CLUTTER_KEY_RELEASE:
		printf("KeyRelease (%d)\n", clutter_event_get_key_code(event));
		break;
	case CLUTTER_MOTION:
		printf("Motion\n");
		break;
	case CLUTTER_ENTER:
		break;
	case CLUTTER_LEAVE:
		break;
	case CLUTTER_BUTTON_PRESS:
		break;
	case CLUTTER_BUTTON_RELEASE:
		break;
	case CLUTTER_SCROLL:
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

void page::bind_wl_compositor(struct wl_client *client, uint32_t version, uint32_t id)
{

}

void page::bind_wl_seat(struct wl_client *client, uint32_t version, uint32_t id)
{

}

void page::bind_wl_data_device_manager(struct wl_client *client, uint32_t version, uint32_t id)
{

}

void page::bind_wl_output(struct wl_client *client, uint32_t version, uint32_t id)
{

}

void page::bind_wl_shell(struct wl_client *client, uint32_t version, uint32_t id)
{

}

void page::main_stage_destroy(ClutterActor *actor)
{
	clutter_main_quit();
}

void page::init(int * argc, char *** argv)
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
		if(page::logfile)
			fclose(page::logfile);
		page::logfile = fopen(log_file_name, "w");
		if(not page::logfile)
			throw except("could not open the log file `%s'", log_file_name);
		// setup the wayland log handler.
		wl_log_set_handler_server(&page::log_printf);
	}


	dpy = wl_display_create();

	/* before clutter_init, give the wayland server display */
	clutter_wayland_set_compositor_display(dpy);

	clutter_init(argc, argv);

	auto device_manager = clutter_device_manager_get_default();
	lookup_device_capabilities(device_manager);

	wayland_init();

	wayland_event_source_init();

	auto display_name = wl_display_add_socket_auto(dpy);
	if (display_name == NULL)
		g_error ("Failed to create socket");

}

void page::run()
{
	clutter_main();
}

} /* namespace page */
