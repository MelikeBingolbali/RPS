#ifndef WEBSOCKET_H
#define WEBSOCKET_H

#define MAX_CLIENTS 2

#include "game.h" // Move ve per_session_data tanımlarını buradan alıyoruz

extern struct lws *clients[MAX_CLIENTS];
extern struct lws *player1;
extern struct lws *player2;
extern int client_count;
extern int game_started;
extern int restart_requests;
extern int player_scores[2];
extern int rounds_played;
extern int moves_left; // Hamle sayısını global olarak tanımlayın

#endif // WEBSOCKET_H
