#include "game.h"
#include <libwebsockets.h>
#include <string.h>
#include "parson.h"

static int player_scores[2] = {0, 0};
static int rounds_played = 0;
#define MAX_CLIENTS 2
#define TOTAL_ROUNDS 10
extern struct lws *clients[MAX_CLIENTS];
