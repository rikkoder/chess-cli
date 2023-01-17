#include <stdlib.h>
#include <string.h>

#include "chess_engine.h"
#include "utilities.h"

#define INIT_ADD_MOVES \
	const tile_t **moves = malloc(MAX_MOVES * sizeof(tile_t*));\
	memset(moves, 0, MAX_MOVES * sizeof(tile_t*));\
 	int idx = 0;\
	face_t color = (tile->piece->face & BLACK);\
	short row = tile->row;\
	short col = tile->col;\
	const tile_t *dest = NULL;\
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

// #define CHECK_AT_DEST (((color & BLACK) && dest->check_for_black) || (!(color & BLACK) && dest->check_for_white))
#define CHECK_AT_DEST (dest->has_check((color & BLACK) ? 1 : 0))

static	const tile_t**	king_moves		(const board_t *board, const tile_t *tile);
static	const tile_t**	queen_moves		(const board_t *board, const tile_t *tile);
static	const tile_t**	rook_moves		(const board_t *board, const tile_t *tile);
static	const tile_t**	bishop_moves	(const board_t *board, const tile_t *tile);
static	const tile_t**	knight_moves	(const board_t *board, const tile_t *tile);
static	const tile_t**	pawn_moves		(const board_t *board, const tile_t *tile);
// static void		find_pin		(const board_t *board, const tile_t *tile);


const tile_t** find_moves(const board_t *board, const tile_t *tile) {
	// no piece at tile to move
	if (tile->piece == NULL)
		return NULL;

	const tile_t **moves = NULL;

// 	find_pin(board, tile);

	int type = 1;
	while (!(tile->piece->face & type)) type <<= 1;

	switch (type) {
		case KING:
			moves = king_moves(board, tile);
			break;
		case QUEEN:
			moves = queen_moves(board, tile);
			break;
		case ROOK:
			moves = rook_moves(board, tile);
			break;
		case BISHOP:
			moves = bishop_moves(board, tile);
			break;
		case KNIGHT:
			moves = knight_moves(board, tile);
			break;
		case PAWN:
			moves = pawn_moves(board, tile);
	}

	// search for pinning tile in moves

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

	// remove check

	// castling
	if ((board->tiles[r1][c1].piece->face & KING)) {
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

	return true;
}


static const tile_t** king_moves (const board_t *board, const tile_t *tile) {
// 	tile_t **moves = malloc(8*sizeof(tile_t*));
// 	memset(moves, 0, sizeof(**moves));

/*	int idx = 0;
 	face_t color = 0;
	color = (tile->piece->face & BLACK);
	short row = tile->row;
	short col = tile->col; */

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
			if (CHECK_AT_DEST)
				continue;

			moves[idx++] = dest;
		}
	}

	// castling
	dest = tile;
	if (CHECK_AT_DEST || tile->piece->is_moved)
		return moves;

	if (!(board->tiles[row][0].piece->is_moved)) {
		bool flag = true;
		dest = &board->tiles[row][1];
		if (dest->piece != NULL)
			flag = false;
		dest = &board->tiles[row][2];
		if (flag && dest->piece != NULL || CHECK_AT_DEST)
			flag = false;
		dest = &board->tiles[row][3];
		if (flag && dest->piece != NULL || CHECK_AT_DEST)
			flag = false;
		if (flag)
			moves[idx++] = &board->tiles[row][col-2];
	} 

	if (!(board->tiles[row][7].piece->is_moved)) {
		bool flag = true;
		dest = &board->tiles[row][5];
		if (dest->piece != NULL || CHECK_AT_DEST)
			flag = false;
		dest = &board->tiles[row][6];
		if (flag && dest->piece != NULL || CHECK_AT_DEST)
			flag = false;
		if (flag)
			moves[idx++] = &board->tiles[row][col+2];
	}

	return moves;
}


static const tile_t** queen_moves (const board_t *board, const tile_t *tile) {
// 	tile_t **moves = malloc(4*7*sizeof(tile_t*));
// 	memset(moves, 0, sizeof(**moves));

/* 	int idx = 0;
	face_t color = 0;
	color = (tile->piece->face & BLACK);
	short row = tile->row;
	short col = tile->col;
	tile_t *dest = NULL;
	piece_t *dest_piece = NULL; */

	INIT_ADD_MOVES;
	ADD_VLINE_MOVES;
	ADD_HLINE_MOVES;
	ADD_PDIAG_MOVES;
	ADD_SDIAG_MOVES;

/* 	// vline
	for (short i = 0; i < 8; i++) {
		// same tile
		if (i == row)
			continue;

		dest = &board->tiles[i][col];
		dest_piece = dest->piece;

		// same color
		if (dest_piece->face != -1 && (dest_piece->face & color))
			break;

		moves[idx++] = dest;

		// opp color
		if (dest_piece->face != -1 && !(dest_piece->face & color))
			break;
	}
 */
/* 	// hline
	for (short j = 0; j < 8; j++) {
		// same tile
		if (j == col)
			continue;

		dest = &board->tiles[row][j];
		dest_piece = dest->piece;

		// same color
		if (dest_piece->face != -1 && (dest_piece->face & color))
			break;

		moves[idx++] = dest;

		// opp color
		if (dest_piece->face != -1 && !(dest_piece->face & color))
			break;
	} */

/* 	// pdiag
	for (short i = row-min(row, col), j = col-min(row, col); i < 8 && j < 8; i++, j++) {
		// same tile
		if (i == row)
			continue;

		dest = &board->tiles[i][j];
		dest_piece = dest->piece;

		// same color
		if (dest_piece->face != -1 && (dest_piece->face & color))
			break;

		moves[idx++] = dest;

		// opp color
		if (dest_piece->face != -1 && !(dest_piece->face & color))
			break;
	} */

/* 	// sdiag
	for (short i = row+min(7-row, col), j = col-min(7-row, col); i >= 0 && j < 8; i--, j++) {
		// same tile
		if (i == row)
			continue;

		dest = &board->tiles[i][j];
		dest_piece = dest->piece;

		// same color
		if (dest_piece->face != -1 && (dest_piece->face & color))
			break;

		moves[idx++] = dest;

		// opp color
		if (dest_piece->face != -1 && !(dest_piece->face & color))
			break;
	} */

	return moves;
}


static const tile_t** rook_moves (const board_t *board, const tile_t *tile) {
// 	tile_t **moves = malloc(2*7*sizeof(tile_t*));
// 	memset(moves, 0, sizeof(**moves));

	INIT_ADD_MOVES;
	ADD_VLINE_MOVES;
	ADD_HLINE_MOVES;

	return moves;
}


static const tile_t** bishop_moves (const board_t *board, const tile_t *tile) {
// 	tile_t **moves = malloc(2*7*sizeof(tile_t*));
// 	memset(moves, 0, sizeof(**moves));

	INIT_ADD_MOVES;
	ADD_PDIAG_MOVES;
	ADD_SDIAG_MOVES;

	return moves;
}


static const tile_t** knight_moves (const board_t *board, const tile_t *tile) {
// 	tile_t **moves = malloc(8*sizeof(tile_t*));
// 	memset(moves, 0, sizeof(**moves));

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


static const tile_t** pawn_moves (const board_t *board, const tile_t *tile) {
// 	tile_t **moves = malloc(4*sizeof(tile_t*));
// 	memset(moves, 0, sizeof(**moves));

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
