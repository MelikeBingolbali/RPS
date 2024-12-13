#include "websocket.h"
#include <libwebsockets.h>

struct lws *clients[MAX_CLIENTS];
struct lws *player1 = NULL;
struct lws *player2 = NULL;
int client_count = 0;
int game_started = 0;
int restart_requests = 0;
int player_scores[2] = {0, 0};
int rounds_played = 0;
int moves_left = 10; // Hamle sayısını başlatın
