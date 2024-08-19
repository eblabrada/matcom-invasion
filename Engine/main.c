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
#include "sprite.h"

// return milliseconds
long get_ticks(void)
{
	struct timespec ts;
	clock_gettime(1, &ts);
	return (long)ts.tv_sec * 1000 + ts.tv_nsec / 1000000;
}

static void draw(struct screen *screen, struct game *game) {
  clear_screen(screen);
	
	draw_game(game, screen);
  
	if (game->state != TITLE)
		print_screen(screen);
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
    start_ticks = get_ticks();

    if (key_pressed != 0) {
      handle_keys(game, key_pressed);
      key_pressed = 0;
    }

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
