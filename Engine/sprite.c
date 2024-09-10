#include "sprite.h"

void move_sprite(struct sprite* sprite, float x, float y) {
	if (!sprite || !sprite->alive) return;

	if (x < 0) x = 0;

	if (x > HEIGHT - sprite->height) x = HEIGHT - sprite->height;

	if (y < 0) y = 0;

	if (y > WIDTH - sprite->width) y = WIDTH - sprite->width;

	sprite->x = x;
	sprite->y = y;
}

void draw_sprite(struct sprite* sprite, struct screen* screen) {
	if (!sprite || !sprite->alive) return;

	if (!sprite->x || !sprite->y)
		throw_error("Sprite position not set in: draw_sprite");

	int x = sprite->x;
	int y = sprite->y;

	for (int i = 0; i < sprite->width; i++) {
		screen->pixels[x * WIDTH + y + i].ascii = sprite->ascii[i];
	}
}

bool collision_sprite(struct sprite* sprite1, struct sprite* sprite2) {
	if (!sprite1 || !sprite2 || !sprite1->alive || !sprite2->alive) return false;

	if (!sprite1->x || !sprite1->y)
		throw_error("Sprite1 position not set in: collision_sprite");

	if (!sprite2->x || !sprite2->y)
		throw_error("Sprite2 position not set in: collision_sprite");

	int x1 = sprite1->x, y1 = sprite1->y;
	int x2 = sprite2->x, y2 = sprite2->y;

	if (y1 < y2 + sprite2->width && y1 + sprite1->width > y2 &&
		x1 < x2 + sprite2->height && x1 + sprite1->height > x2)
		return true;

	return false;
}