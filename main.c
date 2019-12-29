#include <allegro5/allegro.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_image.h>
#include <stdio.h>
#include <stdbool.h>
#include "UI.h"

extern Size screenSize;
extern ALLEGRO_COLOR uiColorBackground;
ALLEGRO_KEYBOARD_STATE kbState;
ALLEGRO_MOUSE_STATE mouseState;

int main(void)
{
	ALLEGRO_DISPLAY* display = NULL;
	ALLEGRO_TIMER* timer = NULL;
	ALLEGRO_EVENT_QUEUE* queue = NULL;
	ALLEGRO_FONT* font = NULL;

	bool loop = true;
	bool redraw = false;

	screenSize.width = DEFAULT_SCREEN_WIDTH;
	screenSize.height = DEFAULT_SCREEN_HEIGHT;

	if (!al_init()) {
		fprintf(stderr, "failed to initialize allegro.\n");
		return -1;
	}

	al_init_font_addon();
	al_init_ttf_addon();
	al_init_primitives_addon();
	al_init_image_addon();
	al_install_keyboard();
	al_install_mouse();
	initUI();

	font = al_load_ttf_font("fonts/UbuntuMono-R.ttf", UI_FONT_SIZE_PT, 0);

	al_set_new_display_flags(ALLEGRO_RESIZABLE);
	display = al_create_display(screenSize.width, screenSize.height);
	if (!display) {
		fprintf(stderr, "failed to create allegro display.\n");
		return -1;
	}
	al_set_window_title(display, "BSpriteTool");

	timer = al_create_timer(1.0 / 60);
	if (!timer) {
		fprintf(stderr, "failed to create allegro timer.\n");
		return -1;
	}

	queue = al_create_event_queue();
	if (!queue) {
		fprintf(stderr, "failed to create allegro event queue.\n");
		return -1;
	}
	al_register_event_source(queue, al_get_keyboard_event_source());
	al_register_event_source(queue, al_get_mouse_event_source());
	al_register_event_source(queue, al_get_display_event_source(display));
	al_register_event_source(queue, al_get_timer_event_source(timer));

	ALLEGRO_EVENT ev;
	al_start_timer(timer);

	while (loop) {
		al_wait_for_event(queue, &ev);
		al_get_keyboard_state(&kbState);
		al_get_mouse_state(&mouseState);
		if (ev.type == ALLEGRO_EVENT_DISPLAY_CLOSE)
			loop = false;
		else if (ev.type == ALLEGRO_EVENT_DISPLAY_RESIZE) {
			al_acknowledge_resize(display);
			screenSize.width = al_get_display_width(display);
			screenSize.height = al_get_display_height(display);
		}
		else if (ev.type == ALLEGRO_EVENT_KEY_DOWN) {
			if (ev.keyboard.keycode == ALLEGRO_KEY_ESCAPE)
				loop = false;
		}
		//else if (ev.type == ALLEGRO_EVENT_MOUSE_BUTTON_UP) {
		//	printf("mouse button up\n");
		//	printf("x: %d, y: %d\n", mouseState.x, mouseState.y);
		//}
		else if (ev.type == ALLEGRO_EVENT_TIMER) {
			redraw = true;
		}

		updateUI(&ev);
		if (redraw && al_is_event_queue_empty(queue)) {
			al_clear_to_color(uiColorBackground);
			drawUI();
			al_flip_display();
			redraw = false;
		}
	}

	cleanupUI();
	al_destroy_font(font);
	al_destroy_timer(timer);
	al_destroy_event_queue(queue);
	al_destroy_display(display);
	return 0;
}