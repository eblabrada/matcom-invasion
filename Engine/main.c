#include <errno.h>
#include <ncurses.h>
#include <pthread.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>  // usleep

#include "game.h"
#include "keys.h"
#include "memory.h"
#include "score.h"
#include "screen.h"
#include "sprite.h"
#include "utils.h"

static void draw(struct screen* screen, struct game* game) {
	clear_screen(screen);

	draw_game(game, screen);

	if (game->state != TITLE) print_screen(screen);
}

static void update(struct game* game, float ftime) {
	struct sprite* ship = &game->ship;
	struct sprite* bullet = &game->bullet;
	struct sprite* aliens = game->aliens;
	int num_aliens = game->num_aliens;

	switch (game->state) {
	case PLAY:
		for (int i = 0; i < num_aliens; i++) {
			if (!aliens[i].alive)
				continue;

			struct sprite* alien = &aliens[i];
			float deltaX = alien->speed * ftime;
			move_sprite(alien, alien->x + deltaX, alien->y);
		}

		if (bullet->alive) {
			float deltaX = bullet->speed * ftime;
			move_sprite(bullet, bullet->x - deltaX, bullet->y);
			if (bullet->x == 0) {
				bullet->alive = false;
			}
		}

		int killed = 0;
		for (int i = 0; i < num_aliens; i++) {
			struct sprite* alien = &aliens[i];
			if (alien->waiting) continue;
 
			if (alien->alive) {
				if (bullet->alive && collision_sprite(bullet, alien)) {
					bullet->alive = false;
					alien->alive = false;
					page_reference(rand() % NUM_PAGES, game);
					game->score += game->level * 10 + 10;
				}

				if (alien->x >= HEIGHT - 3 || collision_sprite(ship, alien)) {
					alien->alive = false;
					ship->lives--;

					if (ship->lives > 0) {
						init_game(game);
					}
					else {
						ship->alive = false;
						game->state = GAME_OVER;
						if (game->score > game->best_score) {
							write_score(game->score);
						}
					}
				}
			}
			else {
				killed++;
			}
		}

		if (killed == num_aliens && ship->alive) {
			game->state = TRANSITION;
			game->score += 100 * game->level;
		}
		break;
	case GAME_OVER:
		break;
	}
}

volatile int key_pressed = 0;

void* keyboard_handler(void* arg) {
	struct game* game = (struct game*)arg;
	while (true) {
		int ch = getch();
		if (ch != ERR) {
			key_pressed = ch;
		}
		if (key_pressed != 0) {
			handle_keys(game, key_pressed);
			key_pressed = 0;
		}
	}
	return NULL;
}

void init_curses() {
	initscr();

	noecho();
	cbreak();
	curs_set(FALSE);
	keypad(stdscr, TRUE);
}

int main(void) {
	init_memory();
	init_curses();

	int ret = 0;
	float elapsed;
	long start_ticks;

	struct screen* screen = NULL;
	struct game* game = NULL;

	game = new_game();
	screen = new_screen();

	if (!screen || !game) {
		ret = ENOMEM;
		goto cleanup;
	}

	game->state = TITLE;
	start_ticks = get_ticks();

	pthread_t keyboard_thread;
	pthread_create(&keyboard_thread, NULL, keyboard_handler, game);

	// main loop
	while (!game->quit) {
		float ftime = (double)(get_ticks() - start_ticks) / 1000.0;
		start_ticks = get_ticks();

		update(game, ftime);
		draw(screen, game);

		elapsed = get_ticks() - start_ticks;  // in milliseconds
		usleep((long)(16.666 - elapsed) * 1000);
	}

	pthread_cancel(keyboard_thread);

	endwin();

cleanup:
	delete_game(game);

	return ret;
}
