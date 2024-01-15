#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "minimax_ab.h"
#include "../chess_engine.h"
#include "../utilities.h"	// min, max and shuffle


typedef struct {
	board_value_t	board_value;
	short			src_tile[2];
	short			dest_tile[2];
} move_t;


static	move_t	_minimax_ab	(board_t *board, history_t *history, board_value_t alpha, board_value_t beta, int depth, board_value_t (*eval_func)(const board_t *board));


bool minimax_ab_play (board_t *board, history_t *history, const minimax_ab_ai_t minimax_ab_ai) {
	time_t t;
	// initialize random no generator
	srand((unsigned int) time(&t));

	// find best move
	move_t best_move = _minimax_ab(board, history, MIN_BOARD_VALUE, MAX_BOARD_VALUE, minimax_ab_ai.depth, minimax_ab_ai.eval_func);

	/* since the move calculated is valid, setting can_be_dest of the target to true to make it work with chess_engine.c:move_piece function. */
	board->tiles[best_move.dest_tile[0]][best_move.dest_tile[1]].can_be_dest = true;
	bool return_value = move_piece(board, best_move.dest_tile, best_move.src_tile, history);
	clear_dest(board);
	return return_value;
}


static move_t _minimax_ab (board_t *board, history_t *history, board_value_t alpha, board_value_t beta, int depth, board_value_t (*eval_func)(const board_t *board)) {
	move_t best_move;
	best_move.board_value = (*eval_func)(board);
	best_move.src_tile[0] = -1;
	best_move.src_tile[1] = -1;
	best_move.dest_tile[0] = -1;
	best_move.dest_tile[1] = -1;
	if (depth == 0 || is_game_finished(board, history))
		return best_move;

	unsigned long long int moves_capacity = 50;
	unsigned long long int moves_count = 0;
	move_t *moves = (move_t *) malloc(moves_capacity * sizeof(move_t));
	for (int i = 0; i < 8; i++) {
		for (int j = 0; j < 8; j++) {
			piece_t *piece = board->tiles[i][j].piece;
			if (piece == NULL || (piece->face & BLACK) != (board->chance & BLACK))
				continue;
			tile_t **piece_moves = find_moves(board, &(board->tiles[i][j]), history);
			if (piece_moves == NULL)
				continue;
			for (int k = 0; k < MAX_MOVES && piece_moves[k] != NULL; k++) {
				if (moves_count < moves_capacity) {
					moves[moves_count].src_tile[0] = i;
					moves[moves_count].src_tile[1] = j;
					moves[moves_count].dest_tile[0] = piece_moves[k]->row;
					moves[moves_count].dest_tile[1] = piece_moves[k]->col;
					moves_count++;
				} else {
					if (moves_capacity >= ULLONG_MAX/2) {
						moves_capacity = ULLONG_MAX;
					} else {
						moves_capacity <<= 1;
					}
					moves = realloc(moves, moves_capacity * sizeof(move_t));
					if (moves == NULL) {
						fprintf(stderr, "catn't run minimax_ab ai... too much possible moves!");
						exit(EXIT_FAILURE);
					}
					moves[moves_count].src_tile[0] = i;
					moves[moves_count].src_tile[1] = j;
					moves[moves_count].dest_tile[0] = piece_moves[k]->row;
					moves[moves_count].dest_tile[1] = piece_moves[k]->col;
					moves_count++;
				}
			}
			free(piece_moves);
		}
	}

	if (moves_count == 0) {
		free(moves);
		return best_move;
	}

	shuffle(moves, moves_count, sizeof(moves[0]));

	if (board->chance == WHITE) {
		best_move.board_value = MIN_BOARD_VALUE;
		for (int i = 0; i < moves_count; i++) {
			// simulate the move
			/* set board->is_fake as true to hint the chess_engine.c that HUMAN moves are being simulated (especially, chess_engine.c:move_piece). set before every move as the board is copied from history after undo and will set board->is_fake to false after copying last real board */
			board->is_fake = true;	
			/* since the moves array contains valid destinations, setting can_be_dest of the target to true to make it work with chess_engine.c:move_piece function. */
// 			tile_t *dest[1] = { &(board->tiles[moves[i].dest_tile[0]][moves[i].dest_tile[1]]) };
// 			set_dest(board, dest);
			board->tiles[moves[i].dest_tile[0]][moves[i].dest_tile[1]].can_be_dest = true;
			if(!move_piece(board, moves[i].dest_tile, moves[i].src_tile, history))
				continue;
			board->tiles[moves[i].dest_tile[0]][moves[i].dest_tile[1]].can_be_dest = false;

			// evaluate
			move_t move_eval = _minimax_ab(board, history, alpha, beta, depth-1, eval_func);
			moves[i].board_value = move_eval.board_value;

			// undo the move
			undo(history);
			copy_board(board, peek_board(history, 0));
			
			// updating best move with move with highest board_value
			// order dependent strategy
			if (moves[i].board_value > best_move.board_value) {
				best_move = moves[i];
			}
			
			/* not updating for equal evaluated moves as they might be result of unoptimized pruned branch */
/* 			else if (moves[i].board_value == best_move.board_value) {
				// use random strategy
				// comment random strategy for reporducibility of same move and finiding bug in ab - pruning
// 				if (rand() % (moves_count/2) == 0) {
					best_move = moves[i];
// 				}
			} */

			// update alpha
			alpha = max(alpha, best_move.board_value);

			// alpha-beta pruning
			if (beta <= alpha)
				break;
		}
	} else {
		best_move.board_value = MAX_BOARD_VALUE;
		for (int i = 0; i < moves_count; i++) {
			// simulate the move
			/* set board->is_fake as true to hint the chess_engine.c that HUMAN moves are being simulated (especially, chess_engine.c:move_piece). set before every move as the board is copied from history after undo and will set board->is_fake to false after copying last real board */
			board->is_fake = true;	
			/* since the moves array contains valid destinations, setting can_be_dest of the target to true to make it work with chess_engine.c:move_piece function. */
// 			tile_t *dest[1] = { &(board->tiles[moves[i].dest_tile[0]][moves[i].dest_tile[1]]) };
// 			set_dest(board, dest);
			board->tiles[moves[i].dest_tile[0]][moves[i].dest_tile[1]].can_be_dest = true;
			if(!move_piece(board, moves[i].dest_tile, moves[i].src_tile, history))
				continue;
			board->tiles[moves[i].dest_tile[0]][moves[i].dest_tile[1]].can_be_dest = false;

			// evaluate
			move_t move_eval = _minimax_ab(board, history, alpha, beta, depth-1, eval_func);
			moves[i].board_value = move_eval.board_value;

			// undo the move
			undo(history);
			copy_board(board, peek_board(history, 0));
			
			// updating best move with move with lowest board_value
			// order dependent strategy
			if (moves[i].board_value < best_move.board_value) {
				best_move = moves[i];
			}
			 
			/* not updating for equal evaluated moves as they might be result of unoptimized pruned branch */
/* 			else if (moves[i].board_value == best_move.board_value) {
				// use random strategy
				// comment random strategy for reporducibility of same move and finiding bug in ab - pruning
// 				if (rand() % (moves_count/2) == 0) {
					best_move = moves[i];
// 				}
			} */

			// update beta
			beta = min(beta, best_move.board_value);

			// alpha-beta pruning
			if (beta <= alpha)
				break;
		}
	}

	// make the board->is_fake false before returning so that game can continue as normal for HUMAN, also AI will set board->is_fake to true before recursively calling this function anyway.
	board->is_fake = false;
	// clear_dest so that no tile remains marked as can_be_dest
	clear_dest(board);
	free(moves);
	return best_move;
}
