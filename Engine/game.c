#include "game.h"

#include <ncurses.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
// #include <time.h>

struct game* new_game(void) {
    struct game* game = (struct game*)malloc(sizeof(struct game));
    if (!game) {
        throw_error("Memory allocation failed!\n");
    }

    game->quit = false;
    return game;
}

void delete_game(struct game* game) { free(game); }

#pragma region DRAW_GAME

static void draw_text(char* text, int x, int y, struct screen* screen) {
    if (!text || !screen) return;

    char* read = text;
    struct pixel* pixel = screen->pixels;

    if (x < 0 || x >= HEIGHT || y < 0 || y >= WIDTH) return;

    pixel += x * WIDTH + y;

    while (y < WIDTH && *read) {
        pixel->ascii = *read++;
        pixel++;
        y++;
    }
}

static void draw_stats(struct game* game, struct screen* screen) {
    if (!game || !screen) return;

    int lvl = game->level;
    int lives = game->ship.lives;
    int score = game->score;
    int best_score = game->best_score;

    struct pixel* pixel;

    {
        char buf[MAX_TEXT_LEN];
        snprintf(buf, MAX_TEXT_LEN, "LEVEL %d", lvl);
        draw_text(buf, 0, 0, screen);
    }

    {
        char buf[MAX_TEXT_LEN];
        snprintf(buf, MAX_TEXT_LEN, "%d LIVES", lives);
        draw_text(buf, 0, WIDTH - LOG10(lives) - 7, screen);
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

    char buf[MAX_TEXT_LEN];
    snprintf(buf, MAX_TEXT_LEN, "%d", best_score);
    draw_text(buf, HEIGHT - 1, 0, screen);
}

void draw_game(struct game* game, struct screen* screen) {
    if (!game || !screen) return;

    struct sprite* ship = &game->ship;
    struct sprite* bullet = &game->bullet;
    struct sprite* aliens = game->aliens;
    struct sprite* aliens_bullets = game->aliens_bullets;

    switch (game->state) {
    case TITLE:
        print_title_screen();
        break;
    case PLAY:
        draw_stats(game, screen);
        draw_sprite(ship, screen);
        draw_sprite(bullet, screen);

        for (int i = 0; i < game->num_aliens; i++) {
            draw_sprite(&aliens[i], screen);
            draw_sprite(&aliens_bullets[i], screen);
        }
        break;
    case TRANSITION:
        draw_stats(game, screen);
        char buf[MAX_TEXT_LEN];
        snprintf(buf, MAX_TEXT_LEN, "NEXT LEVEL %d", game->level + 1);
        draw_text(buf, 10, 31, screen);
        break;
    case GAME_OVER:
        draw_stats(game, screen);
        draw_text("GAME OVER", 10, 35, screen);
        break;
    }
}

void init_game(struct game* game) {
    if (!game) return;

    struct sprite* ship = &game->ship;
    struct sprite* bullet = &game->bullet;
    struct sprite* aliens = game->aliens;
    game->num_aliens = min(50, 10 + 3 * game->level);

    srand(time(NULL));

    ship->x = HEIGHT - 4;
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

    float alien_speed = 0.5 + 0.3 * game->level;
    for (int i = 0; i < game->num_aliens; i++) {
        struct sprite* alien = &aliens[i];

        alien->x = 0;
        alien->y = 0;
        alien->speed = alien_speed;
        alien->alive = false;
        strncpy(alien->ascii, "vQv", MAX_SPRITE_WIDTH);
        alien->width = strlen(alien->ascii);
        alien->height = 1;
        alien->waiting = true;
    }

    generate_aliens_scheduling(game);
}

#pragma endregion

#pragma region SCHEDULING

// Configure "process" scheduling
void generate_aliens_scheduling(struct game* game) {
    struct sprite* aliens = game->aliens;
    int num_aliens = game->num_aliens;

    float arrival_time = 0;
    float waiting_time = 0;
    float remaining_time = 0;

    for (int i = 0; i < num_aliens; ++i) {
        struct sprite* alien = &aliens[i];

        if (i % NUM_PAGES == 0 && i != 0) {
            arrival_time += 2.0 + (double)(rand() % 2000) / 1000.0;
            waiting_time = 0.5 + (double)(rand() % 4000) / 1000.0;
            remaining_time = waiting_time;
        }

        game->waiting_aliens[i] = (struct waiting_alien){
            .alien = alien,
            .arrival_time = arrival_time,
            .waiting_time = waiting_time,
            .remaining_time = remaining_time
        };
    }

    pthread_t thread1;
    if(pthread_create(&thread1, NULL, start_stcf_scheduling, game) != 0) {
        throw_error("Thread creation failed!\n");
    }
    
    struct priority_queue* priority_queue = &game->priority_queue;

    priority_queue->first0 = 0;
    priority_queue->last0 = 0;
    priority_queue->first1 = 0;
    priority_queue->last1 = 0;
    priority_queue->first2 = 0;
    priority_queue->last2 = 0;
    priority_queue->slice_time0 = 1;
    priority_queue->slice_time1 = 2;

    pthread_t thread2;
    if(pthread_create(&thread2, NULL, start_mlfq_scheduling, game) != 0) {
        throw_error("Thread creation failed!\n");
    }
}

// Shortest Time to Completation First Algorithm
void* start_stcf_scheduling(void* arg) {
    struct game* game = (struct game*)arg;
    init_page_queue(game);

    int num_aliens = game->num_aliens;
    int deployed_aliens = 0;

    long start_ticks = get_ticks();
    long last_ticks = get_ticks();
    float elapsed = 0;
    float current_time = 0;

    struct waiting_alien* waiting_alien = NULL;
    struct waiting_alien* i_alien = NULL;
    struct sprite* alien = NULL;

    int alien_id = -1;
    while (deployed_aliens < num_aliens && game->state == PLAY) {
        waiting_alien = NULL;

        current_time = (get_ticks() - start_ticks) / 1000.0;

        for (int i = 0; i < num_aliens; ++i) {
            i_alien = &game->waiting_aliens[i];

            if (i_alien->alien->waiting && i_alien->arrival_time <= current_time) {
                if (!waiting_alien) {
                    waiting_alien = i_alien;
                    alien_id = i;
                }
                else {
                    if (i_alien->remaining_time < waiting_alien->remaining_time) {
                        waiting_alien = i_alien;
                        alien_id = i;
                    }
                }
            }
        }


        if (!waiting_alien) continue;

        elapsed = get_ticks() - last_ticks;  // in milliseconds
        last_ticks = get_ticks();

        waiting_alien->remaining_time -= elapsed / 1000.0;

        if (waiting_alien->remaining_time <= 0) {
            // Deploy alien
            alien = waiting_alien->alien;
            alien->waiting = false;
            alien->alive = true;

            int page = select_page(game);

            alien->x = 1;
            alien->y = page * game->pages[page].offset + game->pages[page].offset / 2 - 1;

            game->priority_queue.waiting_queue[game->priority_queue.last2] = alien_id;
            game->priority_queue.last2++;

            deployed_aliens++;
        }
    }

    pthread_exit(NULL);
}

// Multi Level 
void* start_mlfq_scheduling(void* arg) {
    struct game* game = (struct game*)arg;
    struct priority_queue* priority_queue = &game->priority_queue;
    struct waiting_bullet* q0 = priority_queue->q0;
    struct waiting_bullet* q1 = priority_queue->q1;

    int alive_aliens = 0;
    int index0 = 0, index1 = 0;
    while (game->state == PLAY) {
        for (int i = 0; i < game->num_aliens; ++i) {
            if (game->aliens[i].alive || game->aliens[i].waiting)
                alive_aliens++;
        }

        if (alive_aliens == 0)
            break;

        while (priority_queue->first2 < priority_queue->last2) {
            q0[priority_queue->last0].alien_num = priority_queue->waiting_queue[priority_queue->first2];
            q0[priority_queue->last0].remaining_time = 3;
            priority_queue->last0++;
            priority_queue->first2++;
        }


        if (priority_queue->first0 < priority_queue->last0) {
            if (index0 < priority_queue->last0) {
                int alien_num = q0[index0].alien_num;

                if (game->aliens[alien_num].alive) {
                    q0[index0].remaining_time -= priority_queue->slice_time0;
                    if (q0[index0].remaining_time <= 0) {
                        start_bullet(alien_num, game);
                        priority_queue->first0++;
                        q1[priority_queue->last1].alien_num = alien_num;
                        q1[priority_queue->last1].remaining_time = 4;
                        priority_queue->last1++;
                    }
                    
                    usleep(priority_queue->slice_time0 * 500000);
                }
                else {
                    if (index0 == priority_queue->first0) {
                        priority_queue->first0++;
                    }
                }
                index0++;
            }
            else {
                index0 = priority_queue->first0;
            }
            continue;
        }

        if (priority_queue->first1 < priority_queue->last1) {
            if (index1 < priority_queue->last1) {
                int alien_num = q1[index1].alien_num;

                if (game->aliens[alien_num].alive) {
                    q1[index1].remaining_time -= priority_queue->slice_time1;
                    if (q1[index1].remaining_time <= 0) {
                        start_bullet(alien_num, game);
                        q1[index1].remaining_time = 6;
                    }
                    usleep(priority_queue->slice_time1 * 500000);
                }
                else {
                    if (index1 == priority_queue->first1) {
                        priority_queue->first1++;
                    }
                }
                index1++;
            }
            else {
                index1 = priority_queue->first1;
            }
        }
    }

    pthread_exit(NULL);
}

void start_bullet(int alien_id, struct game* game) {
    struct sprite* bullet = &game->aliens_bullets[alien_id];
    bullet->alive = true;
    bullet->x = game->aliens[alien_id].x + 1;
    bullet->y = game->aliens[alien_id].y;
    bullet->speed = 40.0;
    strncpy(bullet->ascii, "|", MAX_SPRITE_WIDTH);
    bullet->width = strlen(bullet->ascii);
    bullet->height = 1;
}

#pragma endregion

#pragma region PAGINATION

void page_reference(int page_number, struct game* game) {
    push(page_number, game);
}

int select_page(struct game* game) {
    while (true) {
        int ret = pop(game);
        if (game->pages[ret].counter > 1)
            continue;

        page_reference(ret, game);
        return ret;
    }

}

struct page_queue* init_page_queue(struct game* game) {
    struct page_queue* page_queue = &game->page_queue;

    page_queue->first = 0;
    page_queue->last = 0;

    for (int i = 0; i < NUM_PAGES; i++) {
        int page_number = rand() % NUM_PAGES;
        bool repeated = false;

        for (int j = 0; j < i; j++) {
            if (page_queue->queue[j] == page_number) {
                repeated = true;
                break;
            }
        }

        if (repeated) {
            i--;
            continue;
        }

        game->pages[page_number].offset = WIDTH / NUM_PAGES;
        game->pages[page_number].counter = 0;
        push(page_number, game);
    }

    return page_queue;
}

int pop(struct game* game) {
    struct page_queue* page_queue = &game->page_queue;
    if (page_queue->first >= page_queue->last) {
        throw_error("Queue is empty!\n");
        return -1;
    }

    int page_number = page_queue->queue[page_queue->first];
    page_queue->first++;
    game->pages[page_number].counter--;
    return page_number;
}

void push(int page_number, struct game* game) {
    struct page_queue* page_queue = &game->page_queue;
    page_queue->queue[page_queue->last] = page_number;
    page_queue->last++;
    game->pages[page_number].counter++;
}

#pragma endregion