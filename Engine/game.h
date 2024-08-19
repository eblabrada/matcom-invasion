#ifndef GAME_H
#define GAME_H

#include <stdbool.h>
#include <stdlib.h>

#include "screen.h"
#include "sprite.h"
#include "title.h"

#define MAX_NUM_ALIENS 50
#define NUM_ALIENS 10

enum states { TITLE, PLAY, GAME_OVER };

struct game {
  struct sprite ship;
  struct sprite bullet;
  struct sprite aliens[MAX_NUM_ALIENS];
  int state;
  int score;
  bool quit;
};

struct game *new_game(void);
void delete_game(struct game *game);
void draw_game(struct game *game, struct screen *screen);

#endif  // GAME_H