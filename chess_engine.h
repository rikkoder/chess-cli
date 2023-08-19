#ifndef CHESS_ENGINE_H
#define CHESS_ENGINE_H

#include "board.h"
#include "history.h"

#define MAX_MOVES 28

tile_t**		find_moves			(board_t *board, const tile_t *tile, const history_t *history);
bool			move_piece			(board_t *board, short *dest_tile, short *src_tile, history_t *history);
void			clear_dest			(board_t *board);
void			set_dest			(board_t *board, tile_t ** moves);
bool			is_game_finished	(board_t *board, const history_t *history);

#endif
