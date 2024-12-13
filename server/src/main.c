#include <libwebsockets.h>
#include "websocket.h"
#include "websocket_callbacks.h"

#define PORT 8080

int main(void) {
    struct lws_context_creation_info info;
    struct lws_context *context;
    struct lws_protocols protocols[] = {
        {
            .name = "http",
            .callback = callback_ws,
            .per_session_data_size = sizeof(per_session_data),
            .rx_buffer_size = 0,
            .id = 0,
            .user = NULL,
        },
        { NULL, NULL, 0, 0 } // Termination entry
    };

    memset(&info, 0, sizeof(info));
    info.port = PORT;
    info.protocols = protocols;
    info.gid = -1;
    info.uid = -1;

    context = lws_create_context(&info);
    if (context == NULL) {
        lwsl_err("Failed to create context\n");
        return -1;
    }

    lwsl_notice("Starting server...\n");
    while (lws_service(context, 1000) >= 0) {
        // Main loop
    }

    lws_context_destroy(context);
    return 0;
}
