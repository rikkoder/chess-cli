#include "eval_funcs.h"

typedef	short	piece_value_t;

static	const	piece_value_t	piece_values[PIECE_TYPES]	 =	{ 0, 9, 5, 3, 3, 1 };	// in same order as board.c:PIECES


board_value_t piece_value_based_static_eval (const board_t *board) {
	if (board == NULL)
		return 0;

	board_value_t board_value = 0;
	for (int i = 0; i < 8; i++) {
		for (int j = 0; j < 8; j++) {
			piece_t *piece = board->tiles[i][j].piece;
			if (piece == NULL)
				continue;
			int type = 0;
			while (!(piece->face & (1 << type))) type++;
			board_value += (piece->face & BLACK ? -1: 1) * piece_values[type];
		}
	}

	return board_value;
}
