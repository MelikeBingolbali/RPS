#include "websocket_callbacks.h"
#include "websocket.h"
#include "game_logic.h"
#include "game.h"
#include "connection_handler.h"
#include "websocket_helpers.h"
#include "parson.h"
#include <libwebsockets.h>
#include <string.h>

int callback_ws(struct lws *wsi, enum lws_callback_reasons reason, void *user, void *in, size_t len) {
    per_session_data *psd = (per_session_data *)user;

    switch (reason) {
        case LWS_CALLBACK_ESTABLISHED:
            handle_connection_established(wsi, psd);
            if (client_count == MAX_CLIENTS && !game_started) {
                game_started = 1;
                send_start_game_message();
            }
            break;

        case LWS_CALLBACK_RECEIVE:
            lwsl_notice("Received: %.*s\n", (int)len, (char *)in);

            JSON_Value *root_value = json_parse_string((char *)in);
            if (root_value == NULL) {
                lwsl_err("JSON parsing failed\n");
                break;
            }

            JSON_Object *root_object = json_value_get_object(root_value);

            if (json_object_has_value(root_object, "restart") && json_object_get_boolean(root_object, "restart")) {
                if (handle_restart_request(wsi)) {
                    send_start_game_message();
                    lwsl_notice("Game restarted by both players\n");
                }
                json_value_free(root_value); // Bellek sızıntısını önlemek için serbest bırak
            } else if (json_object_has_value(root_object, "move")) {
                const char *move_str = json_object_get_string(root_object, "move");
                if (move_str == NULL) {
                    lwsl_err("JSON object does not contain 'move'\n");
                    json_value_free(root_value);
                    break;
                }

                if (wsi == player1) {
                    lwsl_notice("Player 1 move: %s\n", move_str);
                    if (strcmp(move_str, "rock") == 0) {
                        psd->move = ROCK;
                    } else if (strcmp(move_str, "paper") == 0) {
                        psd->move = PAPER;
                    } else if (strcmp(move_str, "scissors") == 0) {
                        psd->move = SCISSORS;
                    } else {
                        lwsl_notice("Invalid move: %s\n", move_str);
                    }
                } else if (wsi == player2) {
                    lwsl_notice("Player 2 move: %s\n", move_str);
                    if (strcmp(move_str, "rock") == 0) {
                        psd->move = ROCK;
                    } else if (strcmp(move_str, "paper") == 0) {
                        psd->move = PAPER;
                    } else if (strcmp(move_str, "scissors") == 0) {
                        psd->move = SCISSORS;
                    } else {
                        lwsl_notice("Invalid move: %s\n", move_str);
                    }
                }

                // Her iki oyuncunun da hareket yaptığını kontrol edin
                lwsl_notice("Player 1 move value: %d, Player 2 move value: %d\n",
                            ((per_session_data *)lws_wsi_user(player1))->move,
                            ((per_session_data *)lws_wsi_user(player2))->move);

                if (player1 && player2 &&
                    ((per_session_data *)lws_wsi_user(player1))->move != NONE && 
                    ((per_session_data *)lws_wsi_user(player2))->move != NONE) {
                    
                    lwsl_notice("Both players made their moves.\n");
                    process_round_result();
                    ((per_session_data *)lws_wsi_user(player1))->move = NONE;
                    ((per_session_data *)lws_wsi_user(player2))->move = NONE;
                    
                    const char *play_msg = "{\"play\": true, \"player1_score\": %d, \"player2_score\": %d, \"moves_left\": %d}";
                    unsigned char play_buffer[LWS_PRE + 128];
                    unsigned char *pp = &play_buffer[LWS_PRE];
                    size_t play_msg_len = snprintf((char *)pp, sizeof(play_buffer) - LWS_PRE, play_msg, player_scores[0], player_scores[1], moves_left);
                    lwsl_notice("Sending play message: %s\n", pp);
                    lws_write(player1, pp, play_msg_len, LWS_WRITE_TEXT);
                    lws_write(player2, pp, play_msg_len, LWS_WRITE_TEXT);
                } else {
                    lwsl_notice("Waiting for both players to make their moves.\n");
                    const char *waiting_msg = "{\"waiting\": \"%s\"}";
                    unsigned char waiting_buffer[LWS_PRE + 128];
                    unsigned char *pw = &waiting_buffer[LWS_PRE];
                    size_t waiting_msg_len;
                    if (player1 && ((per_session_data *)lws_wsi_user(player1))->move == NONE) {
                        waiting_msg_len = snprintf((char *)pw, sizeof(waiting_buffer) - LWS_PRE, waiting_msg, "Player 1");
                    } else {
                        waiting_msg_len = snprintf((char *)pw, sizeof(waiting_buffer) - LWS_PRE, waiting_msg, "Player 2");
                    }
                    lws_write(player1, pw, waiting_msg_len, LWS_WRITE_TEXT);
                    lws_write(player2, pw, waiting_msg_len, LWS_WRITE_TEXT);
                }
            } else {
                json_value_free(root_value); // Bellek sızıntısını önlemek için serbest bırak
            }
            break;

        case LWS_CALLBACK_WS_PEER_INITIATED_CLOSE:
            lwsl_notice("Peer initiated close\n");
            break;

        case LWS_CALLBACK_CLOSED:
            lwsl_notice("Connection closed\n");
            if (player1 == wsi) {
                player1 = NULL;
                lwsl_notice("Player 1 disconnected\n");
            } else if (player2 == wsi) {
                player2 = NULL;
                lwsl_notice("Player 2 disconnected\n");
            }
            for (int i = 0; i < MAX_CLIENTS; i++) {
                if (clients[i] == wsi) {
                    clients[i] = NULL;
                    client_count--;
                    lwsl_notice("Client count after close: %d\n", client_count);
                    break;
                }
            }
            {
                struct timespec ts = {0, 10000000}; // 10ms
                nanosleep(&ts, NULL);
            }
            break;

        default:
            lwsl_notice("Callback reason: %d\n", reason);
            break;
    }
    return 0;
}
