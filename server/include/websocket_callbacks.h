#ifndef WEBSOCKET_CALLBACKS_H
#define WEBSOCKET_CALLBACKS_H

#include <libwebsockets.h>
#include "game.h" // per_session_data yapısını buradan alıyoruz

int callback_ws(struct lws *wsi, enum lws_callback_reasons reason, void *user, void *in, size_t len);

#endif // WEBSOCKET_CALLBACKS_H
