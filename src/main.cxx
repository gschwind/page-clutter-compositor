
#include <cstdio>
#include <iostream>

#include <clutter/clutter.h>

static
void on_destroy(ClutterActor *actor, gpointer user_data) {
	clutter_main_quit();
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

int main(int argc, char** argv) {

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
	clutter_main();

	return 0;
}

