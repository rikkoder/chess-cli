#ifndef CHESS_ENGINE_H
#define CHESS_ENGINE_H

#include "board.h"

#define MAX_MOVES 28

const tile_t**	find_moves	(const board_t *board, const tile_t *tile);
bool			move_piece	(board_t *board, short *dest_tile, short *src_tile);

#endif
