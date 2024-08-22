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

static void draw_text(char *text, int x, int y, struct screen *screen) {
  if (!text || !screen) return;

  char *read = text;
  struct pixel *pixel = screen->pixels;

  if (x < 0 || x >= HEIGHT || y < 0 || y >= WIDTH) return;

  pixel += x * WIDTH + y;

  while (y < WIDTH && *read) {
    pixel->ascii = *read++;
    pixel++;
    y++;
  }
}

static void draw_stats(struct game *game, struct screen *screen) {
  if (!game || !screen) return;

  int lvl = game->level;
  int lives = game->ship.lives;
  int score = game->score;

  struct pixel *pixel;

  pixel = screen->pixels;
  for (int i = 0; i < lvl; i++) {
    pixel->ascii = 'L';
    pixel++;
  }

  pixel = screen->pixels;
  pixel += WIDTH - 1;
  for (int i = 0; i < lives; i++) {
    pixel->ascii = 'H';
    pixel--;
    pixel->ascii = ' ';
    pixel--;
  }

  pixel = screen->pixels;
  pixel += (HEIGHT * WIDTH) - 1;

  if (score == 0)
    pixel->ascii = '0';
  else {
    while (score > 0) {
      pixel->ascii = (char)(score % 10 + '0');
      pixel--;
      score /= 10;
    }
  }

  // struct sprite *ship = &game->ship;
  // char buf[MAX_TEXT_LEN];
  // snprintf(buf, MAX_TEXT_LEN, "%f %f", ship->x, ship->y);
  // draw_text(buf, HEIGHT - 1, 0, screen);
}

void draw_game(struct game *game, struct screen *screen) {
  if (!game || !screen) return;

  struct sprite *ship = &game->ship;
  struct sprite *bullet = &game->bullet;
  struct sprite *aliens = game->aliens;
  
  switch (game->state) {
    case TITLE:
      print_title_screen();
      break;
    case PLAY:
      draw_stats(game, screen);
      draw_sprite(ship, screen);
      draw_sprite(bullet, screen);

      for (int i = 0; i < NUM_ALIENS; i++) {
        draw_sprite(&aliens[i], screen);
      }

      break;
    case TRANSITION:
      draw_stats(game, screen);
      draw_text("Press Space...", 10, 31, screen);
      break;
    case GAME_OVER:
      draw_stats(game, screen);
      draw_text("GAME OVER", 10, 35, screen);
      break;
  }
}

void init_game(struct game *game) {
  if (!game) return;

  struct sprite *ship = &game->ship;
  struct sprite *bullet = &game->bullet;
  struct sprite *aliens = game->aliens;
  float alien_speed = 15.0 + 1.0 * game->level;

  srand(time(NULL));

  ship->x = HEIGHT - 2;
  ship->y = WIDTH / 2.0 - 3;
  ship->speed = 20.0;
  ship->alive = true;
  strncpy(ship->ascii, "/-I-\\", MAX_SPRITE_WIDTH);
  ship->width = strlen(ship->ascii);
  ship->height = 1;

  bullet->x = 0;
  bullet->y = 0;
  bullet->speed = 50.0;
  bullet->alive = false;
  strncpy(bullet->ascii, "|", MAX_SPRITE_WIDTH);
  bullet->width = strlen(bullet->ascii);
  bullet->height = 1;

  for (int i = 0; i < NUM_ALIENS; i++) {
    struct sprite *alien = &aliens[i];

    alien->speed = alien_speed;
    alien->alive = true;
    strncpy(alien->ascii, "vQv", MAX_SPRITE_WIDTH);
    alien->width = strlen(aliens->ascii);
    alien->height = 1;
    alien->x = 1;
    alien->y = 1 + (aliens->width + 2) * i;
    alien->direction = RIGHT;
  }
}