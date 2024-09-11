#ifndef GAME_H
#define GAME_H

#include <stdbool.h>
#include <stdlib.h>
#include <unistd.h> 
#include <pthread.h>
#include <stdbool.h>

#include "memory.h"
#include "screen.h"
#include "sprite.h"
#include "title.h"
#include "utils.h"

#define MAX_TEXT_LEN 100
#define MAX_NUM_ALIENS 50
#define NUM_PAGES 5

enum states { TITLE, PLAY, TRANSITION, GAME_OVER };

struct page {
	int offset;
	int counter;
};

struct page_queue {
	int queue[2 * MAX_NUM_ALIENS + 2];
	int first;
	int last;
};

struct waiting_bullet {
	int alien_num;
	float remaining_time;
};

struct priority_queue {
	struct waiting_bullet q0[MAX_NUM_ALIENS];
	struct waiting_bullet q1[MAX_NUM_ALIENS];
	int waiting_queue[MAX_NUM_ALIENS];

	int first0, last0;
	int first1, last1;
	int first2, last2;

	int slice_time0;
	int slice_time1;
};

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
	struct sprite aliens_bullets[MAX_NUM_ALIENS];
	struct waiting_alien waiting_aliens[MAX_NUM_ALIENS];
	struct page pages[NUM_PAGES];
	struct page_queue page_queue;
	struct priority_queue priority_queue;
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
void* start_mlfq_scheduling(void* arg);
void start_bullet(int alien_id, struct game* game);

void page_reference(int page_number, struct game* game);
int select_page(struct game* game);
struct page_queue* init_page_queue(struct game* game);
int pop(struct game* game);
void push(int page_number, struct game* game);


#endif  // GAME_H