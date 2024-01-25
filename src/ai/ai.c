#include "ai.h"
#include "../core/history.h"
#include "minimax_ab.h"
#include "eval_funcs.h"


static	minimax_ab_ai_t	get_minimax_ai	(const player_t ai);


bool ai_play (board_t *board, history_t *history) {
	player_t plr1, plr2, ai;
	get_players(history, &plr1, &plr2);
	if (plr1.type == HUMAN)
		ai = plr2;
	else
		ai = plr1;
	return minimax_ab_play(board, history, get_minimax_ai(ai));
}


static minimax_ab_ai_t get_minimax_ai (const player_t ai) {
	const minimax_ab_ai_t	MINIMAX_AB_LVL0 = { 1, piece_value_based_static_eval },
							MINIMAX_AB_LVL1 = { 2, piece_value_based_static_eval },
							MINIMAX_AB_LVL2 = { 3, piece_value_based_static_eval },
							MINIMAX_AB_LVL3 = { 4, piece_value_based_static_eval };

	switch (ai.type) {
		case AI_LVL0:
			return MINIMAX_AB_LVL0;
		case AI_LVL1:
			return MINIMAX_AB_LVL1;
		case AI_LVL2:
			return MINIMAX_AB_LVL2;
		default:
			return MINIMAX_AB_LVL3;
	}
}
