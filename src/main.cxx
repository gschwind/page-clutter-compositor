
#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <iostream>
#include <stdexcept>

#include <wayland-server-core.h>
#include <wayland-server-protocol.h>

#include <clutter/clutter.h>
#include <clutter/wayland/clutter-wayland-compositor.h>
#include <gdk/gdk.h>

#include "exception.hxx"
#include "config_handler.hxx"

#include "wl/wl-compositor.hxx"
#include "wl/wl-seat.hxx"


using namespace page;

typedef struct
{
  GSource source;
  struct wl_display *display;
} WaylandEventSource;

static gboolean
wayland_event_source_prepare (GSource *base, int *timeout)
{
  WaylandEventSource *source = (WaylandEventSource *)base;

  *timeout = -1;

  wl_display_flush_clients (source->display);

  return FALSE;
}

static gboolean
wayland_event_source_dispatch (GSource *base,
                               GSourceFunc callback,
                               void *data)
{
  WaylandEventSource *source = (WaylandEventSource *)base;
  struct wl_event_loop *loop = wl_display_get_event_loop (source->display);

  wl_event_loop_dispatch (loop, 0);

  return TRUE;
}

static GSourceFuncs wayland_event_source_funcs =
{
  wayland_event_source_prepare,
  NULL,
  wayland_event_source_dispatch,
  NULL
};

static GSource *
wayland_event_source_new (struct wl_display *display)
{
  WaylandEventSource *source;
  struct wl_event_loop *loop = wl_display_get_event_loop (display);

  source = (WaylandEventSource *) g_source_new (&wayland_event_source_funcs,
                                                sizeof (WaylandEventSource));
  source->display = display;
  g_source_add_unix_fd (&source->source,
                        wl_event_loop_get_fd (loop),
                        static_cast<GIOCondition>(static_cast<unsigned>(G_IO_IN) | static_cast<unsigned>(G_IO_ERR)));

  return &source->source;
}

static
void on_destroy(ClutterActor *actor, gpointer user_data) {
	clutter_main_quit();
}

uint32_t lookup_device_capabilities(ClutterDeviceManager * device_manager) {

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



static
gboolean page_event_filter(const ClutterEvent *event, gpointer user_data) {

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

static FILE * g_logfile = nullptr;
static void log_print(const char * fmt, va_list args) {
	if(vfprintf(g_logfile, fmt, args) < 0)
		throw std::runtime_error{"could not write the log file"};
}

static void bind_compositor(struct wl_client *client, void *data, uint32_t version, uint32_t id) {
	new wl::wl_compositor{client, version, id};
}

static void bind_seat(struct wl_client *client, void *data, uint32_t version, uint32_t id) {
	auto seat = reinterpret_cast<page_seat*>(data);
	new wl::wl_seat{seat, client, version, id};
}

int main(int argc, char** argv) {
	page::config_handler_t configuration;

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
		if(g_logfile)
			fclose(g_logfile);
		g_logfile = fopen(log_file_name, "w");
		if(not g_logfile)
			throw page::except("could not open the log file `%s'", log_file_name);
	}

	// setup the wayland log handler.
	wl_log_set_handler_server(&log_print);

	auto display = wl_display_create();

	auto wayland_event_source = wayland_event_source_new(display);

	  /* XXX: Here we are setting the wayland event source to have a
	   * slightly lower priority than the X event source, because we are
	   * much more likely to get confused being told about surface changes
	   * relating to X clients when we don't know what's happened to them
	   * according to the X protocol.
	   */
	  g_source_set_priority (wayland_event_source, GDK_PRIORITY_EVENTS + 1);
	  g_source_attach (wayland_event_source, NULL);

	/* before clutter_init, give the wayland server display */
	clutter_wayland_set_compositor_display(display);

	if(clutter_init(&argc, &argv) != CLUTTER_INIT_SUCCESS) {
		printf("fail to initialize clutter");
		return -1;
	}

	auto stage = clutter_stage_new();
	clutter_stage_set_minimum_size(CLUTTER_STAGE(stage), 200, 200);
	clutter_stage_set_title(CLUTTER_STAGE(stage), "X11 output");
	clutter_event_add_filter(CLUTTER_STAGE(stage), page_event_filter, NULL, NULL);

	auto text_actor = clutter_text_new();
	clutter_text_set_text(CLUTTER_TEXT(text_actor), "Clutter test text");

	clutter_actor_add_child(CLUTTER_ACTOR(stage), text_actor);

	g_signal_connect(stage, "destroy", G_CALLBACK (on_destroy), NULL);

	clutter_actor_show(CLUTTER_ACTOR(stage));

	auto device_manager = clutter_device_manager_get_default();
	lookup_device_capabilities(device_manager);

	wl_display_init_shm (display);

	/* TODO: wl_global_bind compositor */
	wl_global_create(display, &wl_compositor_interface, 4, nullptr, &bind_compositor);
	wl_global_create(display, &wl_seat_interface, 6, nullptr, &bind_seat);
	/* TODO: wl_global_bind data_device_manager */

	auto display_name = wl_display_add_socket_auto(display);
	if (display_name == NULL)
		g_error ("Failed to create socket");

	clutter_main();

	return 0;
}

