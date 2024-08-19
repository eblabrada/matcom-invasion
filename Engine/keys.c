#include "keys.h"

#include <ncurses.h>
#include <stdio.h>

void handle_keys_title(struct game *game, char c) {
  switch (c) {
    case 'q':
      game->quit = true;
      break;
    case ' ':
      // start new game
      game->state = PLAY;
      break;
  }
}

void handle_keys_play(struct game *game, char c) {
  struct sprite *ship = &game->ship;
  struct sprite *bullet = &game->bullet;

  switch (c) {
    case 'q':
      game->state = TITLE;
      break;
    case 'w':
      // move up
      move_sprite(ship, ship->x - 1, ship->y);
      break;
    case 's':
      // move down
      move_sprite(ship, ship->x + 1, ship->y);
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
        bullet->y = ship->y + ship->width / 2;
        bullet->alive = true;
      }
      break;
  }
}

void handle_keys_game_over(struct game *game, char c) {
  switch (c) {
    case 'q':
      game->state = TITLE;
      break;
  }
}

void handle_keys(struct game *game, char c) {
  switch (game->state) {
    case TITLE:
      handle_keys_title(game, c);
      break;
    case PLAY:
      handle_keys_play(game, c);
      break;
    case GAME_OVER:
      handle_keys_game_over(game, c);
      break;
  }
}
