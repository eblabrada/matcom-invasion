#include "keys.h"

#include <ncurses.h>
#include <stdio.h>

#include "score.h"

void handle_keys_title(struct game* game, char c) {
	switch (c) {
	case 'q':
		game->quit = true;
		break;
	case ' ':
		// start new game
		game->ship.lives = 3;
		game->score = 0;
		game->level = 1;
		game->best_score = read_score();
		init_game(game);
		game->state = PLAY;
		break;
	}
}

void handle_keys_play(struct game* game, char c) {
	struct sprite* ship = &game->ship;
	struct sprite* bullet = &game->bullet;

	switch (c) {
	case 'q':
		game->state = TITLE;
		break;
	case 'a':
		// move left
		move_sprite(ship, ship->x, ship->y - 1);
		break;
	case 'd':
		// move right
		move_sprite(ship, ship->x, ship->y + 1);
		break;
	case ' ':
		if (ship->alive && !bullet->alive) {
			// shoot
			bullet->x = ship->x - 1;
			bullet->y = ship->y + (float)ship->width / 2.0;
			bullet->alive = true;
		}
		break;
	}
}

void handle_keys_transition(struct game* game, char c) {
	switch (c) {
	case ' ':
		game->level++;
		init_game(game);
		game->state = PLAY;
		break;
	}
}

void handle_keys_game_over(struct game* game, char c) {
	switch (c) {
	case 'q':
		game->state = TITLE;
		break;
	}
}

void handle_keys(struct game* game, char c) {
	switch (game->state) {
	case TITLE:
		handle_keys_title(game, c);
		break;
	case PLAY:
		handle_keys_play(game, c);
		break;
	case TRANSITION:
		handle_keys_transition(game, c);
		break;
	case GAME_OVER:
		handle_keys_game_over(game, c);
		break;
	}
}
