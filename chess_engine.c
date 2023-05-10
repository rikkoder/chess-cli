#include <stdlib.h>
#include <string.h>

#include "chess_engine.h"
#include "utilities.h"

#define INIT_ADD_MOVES \
	tile_t **moves = malloc(MAX_MOVES * sizeof(tile_t*));\
	memset(moves, 0, MAX_MOVES * sizeof(tile_t*));\
 	int idx = 0;\
	face_t color = (tile->piece->face & BLACK);\
	short row = tile->row;\
	short col = tile->col;\
	tile_t *dest = NULL;\
	face_t dest_face;

#define ADD_OR_BREAK \
		dest = &board->tiles[i][j];\
		dest_face = (dest->piece ? dest->piece->face : NO_PIECE);\
		\
		/* same color */\
		if (dest_face != NO_PIECE && ((dest_face & BLACK) == color))\
			break;\
		\
		moves[idx++] = dest;\
		\
		/* opp color */\
		if (dest_face != NO_PIECE && !((dest_face & BLACK) == color))\
			break;

#define ADD_VLINE_MOVES	\
	short j = col;\
	/* -vline */\
	for (short i = row-1; i >= 0; i--) {\
		ADD_OR_BREAK\
	}\
	for (short i = row+1; i < 8; i++) {\
		ADD_OR_BREAK\
	}

#define ADD_HLINE_MOVES \
	short i = row;\
	/* -hline */\
	for (short j = col-1; j >= 0; j--) {\
		ADD_OR_BREAK\
	}\
	/* +hline */\
	for (short j = col+1; j < 8; j++) {\
		ADD_OR_BREAK\
	}

#define ADD_PDIAG_MOVES \
	/* -pdiag */\
	for (short i = row-1, j = col-1; i >= 0 && j >= 0; i--, j--) {\
		ADD_OR_BREAK\
	}\
	/* +pdiag */\
	for (short i = i = row+1, j = col+1; i < 8 && j < 8; i++, j++) {\
		ADD_OR_BREAK\
	}

#define ADD_SDIAG_MOVES \
	/* -sdiag */\
	for (short i = row-1, j = col+1; i >= 0 && j < 8; i--, j++) {\
		ADD_OR_BREAK\
	}\
	/* +sdiag */\
	for (short i = row+1, j = col-1; i < 8 && j >= 0; i++, j--) {\
		ADD_OR_BREAK\
	}

#define CHECK_AT(tile) (tile->has_check[(color & BLACK) ? 1 : 0])

static	tile_t**	king_moves			(board_t *board, const tile_t *tile);
static	tile_t**	queen_moves			(board_t *board, const tile_t *tile);
static	tile_t**	rook_moves			(board_t *board, const tile_t *tile);
static	tile_t**	bishop_moves		(board_t *board, const tile_t *tile);
static	tile_t**	knight_moves		(board_t *board, const tile_t *tile);
static	tile_t**	pawn_moves			(board_t *board, const tile_t *tile);
static	tile_t**	find_all_moves		(board_t *board, const tile_t *tile);
static	void		update_check_map	(board_t *board);
static	bool		is_valid_move		(board_t board, short *dest_tile, short *src_tile);


tile_t** find_moves(board_t *board, const tile_t *tile) {
	tile_t **all_moves = find_all_moves(board, tile);
	if (all_moves == NULL)
		return NULL;

	short src_tile[2] = {tile->row, tile->col};

	tile_t **moves = malloc(MAX_MOVES * sizeof(tile_t*));
	memset(moves, 0, MAX_MOVES * sizeof(tile_t*));
	int k = 0;
	for (int i = 0; i < MAX_MOVES && all_moves[i] != NULL; i++) {
		short dest_tile[2] = {all_moves[i]->row, all_moves[i]->col};
		if (is_valid_move(*board, dest_tile, src_tile))
			moves[k++] = all_moves[i];
	}

	free(all_moves);

	if (k == 0) {
		free(moves);
		moves = NULL;
	}

	return moves;
}


bool move_piece (board_t *board, short *dest_tile, short *src_tile) {
	short r1 = src_tile[0], c1 = src_tile[1], r2 = dest_tile[0], c2 = dest_tile[1];
	// invalid tiles
	if (r1 == INVALID_ROW || r2 == INVALID_ROW || c1 == INVALID_COL || c2 == INVALID_COL || board->tiles[r1][c1].piece == NULL)
		return false;

	// impossible move
	if (!(board->tiles[r2][c2].can_be_dest))
		return false;

	face_t piece_face = board->tiles[r1][c1].piece->face;

	// castling
	if (piece_face & KING) {
		short rook_dest[2] = {r1, INVALID_COL}, rook_src[2] = {r1, INVALID_COL};
		if (c2 == c1-2) {
			rook_src[1] = 0;
			rook_dest[1] = 3;
		} else if (c2 == c1+2) {
			rook_src[1] = 7;
			rook_dest[1] = 5;
		}
		move_piece(board, rook_dest, rook_src);
	}

	board->tiles[r2][c2].piece = board->tiles[r1][c1].piece;
	board->tiles[r1][c1].piece = NULL;
	board->tiles[r2][c2].piece->is_moved = true;


	// update king position in board
	if (piece_face & KING)
		board->kings[piece_face & BLACK ? 1: 0] = &(board->tiles[r2][c2]);

	board->chance = (board->chance == WHITE ? BLACK: WHITE);

	return true;
}


void clear_dest (board_t *board) {
	for (short i = 0; i < 8; i++) {
		for (short j = 0; j < 8; j++) {
			board->tiles[i][j].can_be_dest = false;
		}
	}
}


void set_dest (board_t *board, tile_t **moves) {
	if (moves == NULL)
		return;
	for (int i = 0; i < MAX_MOVES && moves[i] != NULL; i++) {
		moves[i]->can_be_dest = true;
	}
}


bool is_game_finished (board_t *board) {
	color_t color = (board->chance & BLACK ? 1: 0);
	update_check_map(board);
	bool is_check = board->kings[color]->has_check[color];

	for (short i = 0; i < 8; i++) {
		for (short j = 0; j < 8; j++) {
			if (board->tiles[i][j].piece == NULL)
				continue;

			color_t piece_color = board->tiles[i][j].piece->face & BLACK ? 1: 0;
			if (piece_color != color)
				continue;

			tile_t **moves = find_moves(board, &board->tiles[i][j]);
			bool move_exists = (moves != NULL);
			free(moves);

			if (move_exists) {
				board->result = PENDING;
				return false;
			}
		}
	}

	if (is_check)
		board->result = (board->chance == WHITE ? BLACK_WON: WHITE_WON);
	else
		board->result = STALE_MATE;

	return true;
}


static tile_t** king_moves (board_t *board, const tile_t *tile) {
	INIT_ADD_MOVES;

	for (short i = -1; i < 2; i++) {
		for (short j = -1; j < 2; j++) {
			// same tile
			if (i == 0 && j == 0)
				continue;
			// out of board
			if (row+i < 0 || row+i > 7 || col+i < 0 || col+i > 7)
				continue;
			dest = &board->tiles[row+i][col+j];
			dest_face = (dest->piece ? dest->piece->face : NO_PIECE);
			// same color piece
			if (dest_face != NO_PIECE && ((dest_face & BLACK) == color))
				continue;
			// check at dest
			if (CHECK_AT(dest))
				continue;

			moves[idx++] = dest;
		}
	}

	// castling
	if (CHECK_AT(tile) || tile->piece->is_moved)
		return moves;

	if (!(board->tiles[row][0].piece->is_moved)) {
		bool flag = true;
		dest = &board->tiles[row][1];
		if (dest->piece != NULL)
			flag = false;
		dest = &board->tiles[row][2];
		if ((flag && dest->piece != NULL) || CHECK_AT(dest))
			flag = false;
		dest = &board->tiles[row][3];
		if ((flag && dest->piece != NULL) || CHECK_AT(dest))
			flag = false;
		if (flag)
			moves[idx++] = &board->tiles[row][col-2];
	} 

	if (!(board->tiles[row][7].piece->is_moved)) {
		bool flag = true;
		dest = &board->tiles[row][5];
		if (dest->piece != NULL || CHECK_AT(dest))
			flag = false;
		dest = &board->tiles[row][6];
		if ((flag && dest->piece != NULL) || CHECK_AT(dest))
			flag = false;
		if (flag)
			moves[idx++] = &board->tiles[row][col+2];
	}

	return moves;
}


static tile_t** queen_moves (board_t *board, const tile_t *tile) {
	INIT_ADD_MOVES;

	ADD_VLINE_MOVES;
	ADD_HLINE_MOVES;
	ADD_PDIAG_MOVES;
	ADD_SDIAG_MOVES;

	return moves;
}


static tile_t** rook_moves (board_t *board, const tile_t *tile) {
	INIT_ADD_MOVES;

	ADD_VLINE_MOVES;
	ADD_HLINE_MOVES;

	return moves;
}


static tile_t** bishop_moves (board_t *board, const tile_t *tile) {
	INIT_ADD_MOVES;

	ADD_PDIAG_MOVES;
	ADD_SDIAG_MOVES;

	return moves;
}


static tile_t** knight_moves (board_t *board, const tile_t *tile) {
	INIT_ADD_MOVES;

	for (short i = -2; i <= 2; i++) {
		for (short j = -2; j <= 2; j++) {
			// lines or diags
			if (i == 0 || j == 0 || i == j || i == -j)
				continue;

			// out of board
			if (row+i < 0 || row+i > 7 || col+j < 0 || col+j > 7)
				continue;

			dest = &board->tiles[row+i][col+j];
			dest_face = (dest->piece ? dest->piece->face : NO_PIECE);

			// same color
			if (dest_face != NO_PIECE && ((dest_face & BLACK) == color))
				continue;

			moves[idx++] = dest;
		}
	}

	return moves;
}


static tile_t** pawn_moves (board_t *board, const tile_t *tile) {
	INIT_ADD_MOVES;

	if (color == BLACK) {
		// double step
		if (row == 6 && board->tiles[row-2][col].piece == NULL)
			moves[idx++] = &board->tiles[row-2][col];

		// single step
		dest = &board->tiles[row-1][col];
		if (dest->piece == NULL)
			moves[idx++] = dest;

		// attack moves
		for (short i = -1; i < 2; i++) {
			if (i == 0 || col + i < 0 || col + i > 7)
				continue;
			dest = &board->tiles[row-1][col+i];
			dest_face = (dest->piece ? dest->piece->face : NO_PIECE);
			if (dest_face != NO_PIECE && !((dest_face & BLACK) == color))
				moves[idx++] = dest;
		}

		// en passant
	} else {
		// double step
		if (row == 1 && board->tiles[row+2][col].piece == NULL)
			moves[idx++] = &board->tiles[row+2][col];

		// single step
		dest = &board->tiles[row+1][col];
		if (dest->piece == NULL)
			moves[idx++] = dest;

		// attack moves
		for (short i = -1; i < 2; i++) {
			if (i == 0 || col + i < 0 || col + i > 7)
				continue;
			dest = &board->tiles[row+1][col+i];
			dest_face = (dest->piece ? dest->piece->face : NO_PIECE);
			if (dest_face != NO_PIECE && !((dest_face & BLACK) == color))
				moves[idx++] = dest;
		}

		// en passant
	}

	return moves;
}


static tile_t** find_all_moves (board_t *board, const tile_t *tile) {
	// no piece at tile to move
	if (tile->piece == NULL)
		return NULL;

	tile_t **all_moves = NULL;

	int type = 1;
	while (!(tile->piece->face & type)) type <<= 1;

	switch (type) {
		case KING:
			all_moves = king_moves(board, tile);
			break;
		case QUEEN:
			all_moves = queen_moves(board, tile);
			break;
		case ROOK:
			all_moves = rook_moves(board, tile);
			break;
		case BISHOP:
			all_moves = bishop_moves(board, tile);
			break;
		case KNIGHT:
			all_moves = knight_moves(board, tile);
			break;
		case PAWN:
			all_moves = pawn_moves(board, tile);
	}

	return all_moves;
}


static void update_check_map (board_t *board) {
	// make each tile safe
	for (short i = 0; i < 8; i++) {
		for (short j = 0; j < 8; j++) {
			board->tiles[i][j].has_check[0] = false;
			board->tiles[i][j].has_check[1] = false;
		}
	}

	for (short i = 0; i < 8; i++) {
		for (short j = 0; j < 8; j++) {
			tile_t *tile = &(board->tiles[i][j]);
			if (tile->piece == NULL)
				continue;

			color_t color = (tile->piece->face & BLACK ? 1: 0);

			tile_t **moves = NULL;
			int type = 1;
			while (!(tile->piece->face & type)) type <<= 1;
			if (type == PAWN) {
				moves = malloc(2 * sizeof(tile_t*));
				memset(moves, 0, 2 * sizeof(tile_t*));

				int row = i + 1;
				if (color == 1)
					row = i - 1;
				
				if (j == 0)
					moves[0] = &(board->tiles[row][j+1]);
				else if (j == 7)
					moves[0] = &(board->tiles[row][j-1]);
				else {
					moves[0] = &(board->tiles[row][j-1]);
					moves[1] = &(board->tiles[row][j+1]);
				}
			} else {
				moves = find_all_moves(board, tile);
			}


			for (int k = 0; k < (type == PAWN ? 2: MAX_MOVES) && moves[k] != NULL; k++) {
				moves[k]->has_check[!color] = true;
			}
		}
	}
}


static bool is_valid_move (board_t board, short *dest_tile, short *src_tile) {
	color_t color = board.tiles[src_tile[0]][src_tile[1]].piece->face & BLACK ? 1: 0;
	for (int i=0; i<2; i++)
		board.kings[i] = &board.tiles[board.kings[i]->row][board.kings[i]->col];
	tile_t *src= &board.tiles[src_tile[0]][src_tile[1]];
	tile_t *dest = &board.tiles[dest_tile[0]][dest_tile[1]];

	if (src->piece->face & KING)
		board.kings[color] = dest;

	dest->piece = src->piece;
	src->piece = NULL;

	update_check_map(&board);

	return !(board.kings[color]->has_check[color]);
}
