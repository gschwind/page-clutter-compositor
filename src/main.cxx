
#include <cstdio>
#include <iostream>

#include <clutter/clutter.h>

static
void on_destroy(ClutterActor *actor, gpointer user_data) {
	clutter_main_quit();
}

int main(int argc, char** argv) {

	if(clutter_init(&argc, &argv) != CLUTTER_INIT_SUCCESS) {
		printf("fail to initialize clutter");
		return -1;
	}

	auto stage = clutter_stage_new();
	clutter_stage_set_minimum_size(CLUTTER_STAGE(stage), 200, 200);
	clutter_stage_set_title(CLUTTER_STAGE(stage), "X11 output");

	auto text_actor = clutter_text_new();
	clutter_text_set_text(CLUTTER_TEXT(text_actor), "Clutter test text");

	clutter_actor_add_child(CLUTTER_ACTOR(stage), text_actor);

	g_signal_connect(stage, "destroy", G_CALLBACK (on_destroy), NULL);

	clutter_actor_show(CLUTTER_ACTOR(stage));
	clutter_main();

	return 0;
}

