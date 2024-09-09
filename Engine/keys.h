#ifndef KEYS_H
#define KEYS_H

#include "game.h"

void handle_keys_title(struct game* game, char c);
void handle_keys_play(struct game* game, char c);
void handle_keys_transition(struct game* game, char c);
void handle_keys_game_over(struct game* game, char c);
void handle_keys(struct game* game, char c);

#endif  // KEYS_H