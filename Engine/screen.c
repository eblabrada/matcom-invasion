#include "screen.h"

#include <ncurses.h>
#include <stdio.h>
#include <stdlib.h>

struct screen *new_screen(void) {
  struct screen *screen = (struct screen*)malloc(sizeof(struct screen));
  if (!screen) return NULL;
  return screen;
}

void delete_screen(struct screen *screen) { free(screen); }

void clear_screen(struct screen *screen) {
  if (!screen) return;

  clear();
  refresh();

  struct pixel *pixel = screen->pixels;

  for (int i = 0; i < HEIGHT * WIDTH; i++) {
    pixel->ascii = ' ';
    pixel++;
  }
}

void print_screen(struct screen *screen) {
  if (!screen) return;

  struct pixel *pixel = screen->pixels;

  clear();
  for (int x = 0; x < HEIGHT; x++) {
    for (int y = 0; y < WIDTH; y++) {
      mvprintw(x, y, "%c", pixel->ascii);
      pixel++;
    }
  }
  refresh();
}