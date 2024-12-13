#ifndef WEBSOCKET_HELPERS_H
#define WEBSOCKET_HELPERS_H

#include <libwebsockets.h>

void send_role_message(struct lws *wsi, const char *role);
void reset_game();
void send_start_game_message();

#endif // WEBSOCKET_HELPERS_H
