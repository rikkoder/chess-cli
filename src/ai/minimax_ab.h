#ifndef MINIMAX_AB_H
#define MINIMAX_AB_H

#include "eval_funcs.h"
#include "../board.h"
#include "../history.h"


typedef struct {
	int depth;
	board_value_t (*eval_func)(const board_t *board);
} minimax_ab_ai_t;


bool	minimax_ab_play	(board_t *board, history_t *history, const minimax_ab_ai_t minimax_ab_ai);

#endif
