#include <errno.h>
#include <ncurses.h>
#include <pthread.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>  // usleep

#include "game.h"
#include "keys.h"
#include "screen.h"
#include "sprite.h"

// return milliseconds
long get_ticks(void) {
  struct timespec ts;
  clock_gettime(1, &ts);
  return (long)ts.tv_sec * 1000 + ts.tv_nsec / 1000000;
}

static void draw(struct screen *screen, struct game *game) {
  clear_screen(screen);
  
  draw_game(game, screen);

  if (game->state != TITLE) print_screen(screen);
}

static void update(struct game *game, float ftime) {
  struct sprite *ship = &game->ship;
  struct sprite *bullet = &game->bullet;
  struct sprite *aliens = game->aliens;

  switch (game->state) {
    case PLAY:
      for (int i = 0; i < NUM_ALIENS; i++) {
        struct sprite *alien = &aliens[i];
        float deltaY = alien->speed * ftime;

        if (alien->direction == LEFT) deltaY *= -1;

        move_sprite(alien, alien->x, alien->y + deltaY);

        if (alien->y == WIDTH - alien->width) {
          move_sprite(alien, alien->x + 2, alien->y);
          alien->direction = LEFT;
          alien->speed += 5;
        }

        if (alien->y == 0) {
          move_sprite(alien, alien->x + 2, alien->y);
          alien->direction = RIGHT;
        }
      }

      if (bullet->alive) {
        float deltaX = bullet->speed * ftime;
        move_sprite(bullet, bullet->x - deltaX, bullet->y);
        if (bullet->x == 0) {
          bullet->alive = false;
        }
      }

      int killed = 0;
      for (int i = 0; i < NUM_ALIENS; i++) {
        struct sprite *alien = &aliens[i];

        if (alien->alive) {
          if (bullet->alive && collision_sprite(bullet, alien)) {
            bullet->alive = alien->alive = false;
            game->score += game->level * 10 + 10;
          }

          if (collision_sprite(ship, alien)) {
            alien->alive = false;
            ship->lives--;

            if (ship->lives > 0) {
              init_game(game);
            } else {
              ship->alive = false;
              game->state = GAME_OVER;
            }
          }
        } else {
          killed++;
        }
      }

      if (killed == NUM_ALIENS && ship->alive) {
        game->state = TRANSITION;
        game->score += 100 * game->level;
      }

      break;
    case GAME_OVER:
      break;
  }
}

volatile int key_pressed = 0;

void *keyboard_handler(void *arg) {
  while (true) {
    int ch = getch();
    if (ch != ERR) {
      key_pressed = ch;
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
  init_curses();

  int ret = 0;
  float elapsed;
  long start_ticks;

  struct screen *screen = NULL;
  struct game *game = NULL;

  game = new_game();
  screen = new_screen();

  if (!screen || !game) {
    ret = ENOMEM;
    goto cleanup;
  }

  game->state = TITLE;
  start_ticks = get_ticks();

  pthread_t keyboard_thread;
  pthread_create(&keyboard_thread, NULL, keyboard_handler, NULL);

  // main loop
  while (!game->quit) {
    float ftime = (get_ticks() - start_ticks) / 1000.0;
    start_ticks = get_ticks();

    if (key_pressed != 0) {
      handle_keys(game, key_pressed);
      key_pressed = 0;
    }

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
