#ifndef GAME_LOGIC_H
#define GAME_LOGIC_H

#include "game.h" // Move ve per_session_data tanımını buradan alıyoruz

void process_round_result();
int evaluate_moves(Move move1, Move move2);

#endif // GAME_LOGIC_H
