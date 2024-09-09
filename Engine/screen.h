#ifndef SCREEN_H
#define SCREEN_H

#include "memory.h"

#define WIDTH 80
#define HEIGHT 25

struct pixel {
    char ascii;
};

struct screen {
    struct pixel pixels[HEIGHT * WIDTH];
};

struct screen* new_screen(void);
void delete_screen(struct screen* screen);
void clear_screen(struct screen* screen);
void print_screen(struct screen* screen);

#endif  // SCREEN_H
