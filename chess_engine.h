#ifndef CHESS_ENGINE_H
#define CHESS_ENGINE_H

#include "board.h"

#define MAX_MOVES 28

const tile_t**	find_moves			(const board_t *board, const tile_t *tile);
bool			move_piece			(board_t *board, short *dest_tile, short *src_tile);
void			clear_dest			(board_t *board);
void			set_dest			(board_t *board, tile_t ** moves);
bool			is_game_finished	(board_t *board);
#endif
