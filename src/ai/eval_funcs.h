#ifndef EVAL_FUNCS_H
#define EVAL_FUNCS_H

#include <limits.h>

#include "../board.h"

#define	MIN_BOARD_VALUE	INT_MIN
#define MAX_BOARD_VALUE	INT_MAX


typedef	int	board_value_t;

board_value_t	piece_value_based_static_eval	(const board_t *board);

#endif
