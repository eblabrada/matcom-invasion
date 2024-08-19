#include "screen.h"

#include <ncurses.h>
#include <stdio.h>
#include <stdlib.h>

char* convert_char_to_str(char c) {
  char *str = (char *)malloc(2 * sizeof(char));
  str[0] = c;
  str[1] = '\0';
  return str;
}

struct screen *new_screen(void) {
  struct screen *screen = malloc(sizeof(*screen));
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
      char *cur_pixel = convert_char_to_str(pixel->ascii);
      mvprintw(x, y, cur_pixel);
      free(cur_pixel);
      pixel++;
    }
  }
  refresh();
}