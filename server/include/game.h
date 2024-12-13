#ifndef GAME_H
#define GAME_H

typedef enum {
    NONE,
    ROCK,
    PAPER,
    SCISSORS
} Move;

typedef struct per_session_data {
    Move move;
} per_session_data;

#endif // GAME_H
