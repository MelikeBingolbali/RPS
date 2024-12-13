#ifndef CONNECTION_HANDLER_H
#define CONNECTION_HANDLER_H

#include <libwebsockets.h>
#include "game.h" // per_session_data yapısını buradan alıyoruz

void handle_connection_established(struct lws *wsi, per_session_data *psd);

#endif // CONNECTION_HANDLER_H
