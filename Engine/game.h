#ifndef GAME_H
#define GAME_H

#include <stdbool.h>
#include <stdlib.h>

#include "memory.h"
#include "screen.h"
#include "sprite.h"
#include "title.h"
#include "utils.h"

#define MAX_TEXT_LEN 100
#define MAX_NUM_ALIENS 50

enum states { TITLE, PLAY, TRANSITION, GAME_OVER };

struct waiting_alien {
    struct sprite* alien;
    float arrival_time;
    float waiting_time;
    float remaining_time;
};

struct game {
	struct sprite ship;
	struct sprite bullet;
	struct sprite aliens[MAX_NUM_ALIENS];
	struct waiting_alien waiting_aliens[MAX_NUM_ALIENS];
	int num_aliens;
	int state;
	int score, best_score;
	int level;
	bool quit;
};

struct game* new_game(void);
void delete_game(struct game* game);
void draw_game(struct game* game, struct screen* screen);
void init_game(struct game* game);

void generate_aliens_scheduling(struct game* game);
void* start_stcf_scheduling(void* arg);

#endif  // GAME_H