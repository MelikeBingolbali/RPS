#include "game_logic.h"
#include "websocket.h"
#include <libwebsockets.h>

int evaluate_moves(Move move1, Move move2)
{
    if (move1 == move2)
    {
        return 0; // Draw
    }
    else if ((move1 == ROCK && move2 == SCISSORS) ||
             (move1 == PAPER && move2 == ROCK) ||
             (move1 == SCISSORS && move2 == PAPER))
    {
        return 1; // Player 1 wins
    }
    else
    {
        return 2; // Player 2 wins
    }
}

void process_round_result()
{
    per_session_data *psd0 = (per_session_data *)lws_wsi_user(player1);
    per_session_data *psd1 = (per_session_data *)lws_wsi_user(player2);

    lwsl_notice("Player 1 move: %d, Player 2 move: %d\n", psd0->move, psd1->move);

    int result = evaluate_moves(psd0->move, psd1->move);

    const char *result_msg;
    if (moves_left > 0)
    {
        if (result == 0)
        {
            lwsl_notice("It's a draw!\n");
            result_msg = "{\"result\": \"It's a draw!\", \"player1_score\": %d, \"player2_score\": %d, \"moves_left\": %d}";
        }
        else if (result == 1)
        {
            lwsl_notice("Player 1 wins this round!\n");
            player_scores[0]++;
            result_msg = "{\"result\": \"Player 1 wins this round!\", \"player1_score\": %d, \"player2_score\": %d, \"moves_left\": %d}";
        }
        else if (result == 2)
        {
            lwsl_notice("Player 2 wins this round!\n");
            player_scores[1]++;
            result_msg = "{\"result\": \"Player 2 wins this round!\", \"player1_score\": %d, \"player2_score\": %d, \"moves_left\": %d}";
        }

        rounds_played++;
        moves_left--;
        if (moves_left == 0)
        {
            lwsl_notice("Game over!\n");
            result_msg = "{\"result\": \"Game over!\", \"player1_score\": %d, \"player2_score\": %d, \"moves_left\": %d, \"game_over\": true}";
        }
    }

    lwsl_notice("Player 1 Score: %d, Player 2 Score: %d\n", player_scores[0], player_scores[1]);

    unsigned char buffer[LWS_PRE + 256];
    unsigned char *p = &buffer[LWS_PRE];
    size_t msg_len = snprintf((char *)p, sizeof(buffer) - LWS_PRE, result_msg, player_scores[0], player_scores[1], moves_left);
    lwsl_notice("Sending result message: %s\n", p);
    lws_write(player1, p, msg_len, LWS_WRITE_TEXT);
    lws_write(player2, p, msg_len, LWS_WRITE_TEXT);
}
