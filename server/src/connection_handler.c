#include "connection_handler.h"
#include "websocket.h"
#include "websocket_helpers.h"
#include <libwebsockets.h>
#include <string.h>

void handle_connection_established(struct lws *wsi, struct per_session_data *psd) {
    lwsl_notice("Connection established\n");
    psd->move = NONE;

    for (int i = 0; i < MAX_CLIENTS; i++) {
        if (clients[i] == NULL) {
            clients[i] = wsi;
            if (player1 == NULL) {
                player1 = wsi;
                send_role_message(wsi, "Player 1");
                lwsl_notice("Assigned Player 1\n");
            } else if (player2 == NULL) {
                player2 = wsi;
                send_role_message(wsi, "Player 2");
                lwsl_notice("Assigned Player 2\n");
            }
            client_count++;
            lwsl_notice("Client count is now: %d\n", client_count);
            break;
        }
    }
}
