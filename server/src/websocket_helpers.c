#include "websocket_helpers.h"
#include "websocket.h"
#include "parson.h"
#include "game.h"
#include <string.h>

static int player1_restart_request = 0; 
static int player2_restart_request = 0; 

void reset_game() {
    lwsl_notice("Resetting game\n");

    // Oyuncu hareketlerini ve skorları sıfırlayın
    player_scores[0] = 0;
    player_scores[1] = 0;
    moves_left = 10;
    rounds_played = 0;

    if (player1) {
        ((per_session_data *)lws_wsi_user(player1))->move = NONE;
    }
    if (player2) {
        ((per_session_data *)lws_wsi_user(player2))->move = NONE;
    }

    // İlgili diğer verileri sıfırlayın
    game_started = 0;
    player1_restart_request = 0;  // Player 1 restart isteğini sıfırla
    player2_restart_request = 0;  // Player 2 restart isteğini sıfırla
}

int handle_restart_request(struct lws *wsi) {
    if (wsi == player1) {
        player1_restart_request = 1;
    } else if (wsi == player2) {
        player2_restart_request = 1;
    }

    lwsl_notice("Restart request received. Player 1: %d, Player 2: %d\n", player1_restart_request, player2_restart_request);

    if (player1_restart_request && player2_restart_request) {
        reset_game();
        return 1; // Oyun yeniden başlatıldı
    }

    // Tüm oyuncuların yeniden başlatma isteğini bekleyin
    const char *waiting_msg = "{\"waiting_restart\": true}";
    unsigned char waiting_buffer[LWS_PRE + 128];
    unsigned char *pw = &waiting_buffer[LWS_PRE];
    size_t waiting_msg_len = snprintf((char *)pw, sizeof(waiting_buffer) - LWS_PRE, "%s", waiting_msg);
    lws_write(player1, pw, waiting_msg_len, LWS_WRITE_TEXT);
    lws_write(player2, pw, waiting_msg_len, LWS_WRITE_TEXT);

    return 0; // Henüz yeniden başlatılmadı
}

void send_role_message(struct lws *wsi, const char *role) {
    JSON_Value *root_value = json_value_init_object();
    JSON_Object *root_object = json_value_get_object(root_value);
    json_object_set_string(root_object, "role", role);
    char *message = json_serialize_to_string(root_value);

    unsigned char buffer[LWS_PRE + 128];
    unsigned char *p = &buffer[LWS_PRE];
    size_t msg_len = strlen(message);
    memcpy(p, message, msg_len);
    lws_write(wsi, p, msg_len, LWS_WRITE_TEXT);

    json_free_serialized_string(message);
    json_value_free(root_value);
}

// void reset_game() {
//     lwsl_notice("Resetting game\n");

//     // Oyuncu hareketlerini ve skorları sıfırlayın
//     player_scores[0] = 0;
//     player_scores[1] = 0;
//     moves_left = 10;
//     rounds_played = 0;

//     if (player1) {
//         ((per_session_data *)lws_wsi_user(player1))->move = NONE;
//     }
//     if (player2) {
//         ((per_session_data *)lws_wsi_user(player2))->move = NONE;
//     }

//     // İlgili diğer verileri sıfırlayın
//     game_started = 0;
// }


// int handle_restart_request(struct lws *wsi) {
//     if (wsi == player1) {
//         player1_restart_requested = 1;
//     } else if (wsi == player2) {
//         player2_restart_requested = 1;
//     }

//     if (player1_restart_requested && player2_restart_requested) {
//         player1_restart_requested = 0;
//         player2_restart_requested = 0;
//         reset_game();
//         return 1; // Oyun yeniden başlatıldı
//     }

//     // Tüm oyuncuların yeniden başlatma isteğini bekleyin
//     const char *waiting_msg = "{\"waiting_restart\": true}";
//     unsigned char waiting_buffer[LWS_PRE + 128];
//     unsigned char *pw = &waiting_buffer[LWS_PRE];
//     size_t waiting_msg_len = snprintf((char *)pw, sizeof(waiting_buffer) - LWS_PRE, "%s", waiting_msg);
//     lws_write(player1, pw, waiting_msg_len, LWS_WRITE_TEXT);
//     lws_write(player2, pw, waiting_msg_len, LWS_WRITE_TEXT);

//     return 0; // Henüz yeniden başlatılmadı
// }

void send_start_game_message() {
    const char *start_msg = "{\"message\": \"Game starting\"}";
    for (int i = 0; i < MAX_CLIENTS; i++) {
        if (clients[i] != NULL) {
            unsigned char buffer[LWS_PRE + 128];
            unsigned char *p = &buffer[LWS_PRE];
            size_t msg_len = strlen(start_msg);
            memcpy(p, start_msg, msg_len);
            lws_write(clients[i], p, msg_len, LWS_WRITE_TEXT);
        }
    }
}

