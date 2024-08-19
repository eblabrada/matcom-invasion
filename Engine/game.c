#include "game.h"

#include <assert.h>
#include <ncurses.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "sprite.h"

struct game *new_game(void) {
  struct game *game = malloc(sizeof(*game));
  if (!game) return NULL;

  game->quit = false;
  return game;
}

void delete_game(struct game *game) { free(game); }

void draw_game(struct game *game, struct screen *screen) {
  if (!game || !screen) return;

  switch (game->state) {
    case TITLE:
      print_title_screen();
      break;
    case PLAY:

      break;
    case GAME_OVER:
      break;
  }
}