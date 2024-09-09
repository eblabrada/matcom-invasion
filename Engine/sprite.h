#ifndef SPRITE_H
#define SPRITE_H

#include <stdbool.h>
#include <stdlib.h>

#include "memory.h"
#include "screen.h"

#define MAX_SPRITE_WIDTH 10

enum directions { LEFT, RIGHT };

struct sprite {
	float x, y;
	int width, height;
	float speed;
	bool alive;
	int lives;
	char ascii[MAX_SPRITE_WIDTH];
	int direction;
};

void move_sprite(struct sprite* sprite, float x, float y);
void draw_sprite(struct sprite* sprite, struct screen* screen);
bool collision_sprite(struct sprite* sprite1, struct sprite* sprite2);

#endif  // SPRITE_H