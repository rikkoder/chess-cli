#include <string.h>
#include <stdlib.h>

#include "board.h"

const wchar_t PIECES[2][2][6] = {
	{
		{'K', 'Q', 'R', 'N', 'B', 'P'},
		{'k', 'q', 'r', 'n', 'b', 'p'}
	},
	{
		{L'\u2654', L'\u2655', L'\u2656', L'\u2657', L'\u2658', L'\u2659'},
		{L'\u265A', L'\u265B', L'\u265C', L'\u265D', L'\u265E', L'\u265F'}
	}
};

piece_t* init_piece(short i, short j) {
	// no piece
	if (i > 1 && i < 6)
		return NULL;

	piece_t *piece = malloc(sizeof(piece_t));
	memset(piece, 0, sizeof(piece_t));

	piece->face |= UNICODE;
	if (i >= 6) piece->face |= BLACK;
	if (i == 1 || i == 6) {
		piece->face |= PAWN;
		return piece;
	}

	switch (j) {
		case 0:
		case 7:
			piece->face |= ROOK;
			break;
		case 1:
		case 6:
			piece->face |= KNIGHT;
			break;
		case 2:
		case 5:
			piece->face |= BISHOP;
			break;
		case 3:
			piece->face |= QUEEN;
			break;
		case 4:
			piece->face |= KING;
	}

	return piece;
}

wchar_t get_piece_face(const piece_t *piece) {
	if (piece == NULL)
		return L' ';
	int type = 0;
	while (!(piece->face & (1 << type))) type++;
	return PIECES[(bool) (piece->face & UNICODE)][(bool) (piece->face & BLACK)][type];
}

void init_board(board_t *board) {
	memset(board, 0, sizeof(*board));
	for (short i=0; i<8; i++) {
		for (short j=0; j<8; j++) {
			board->tiles[i][j].row = i;
			board->tiles[i][j].col = j;
			board->tiles[i][j].piece = init_piece(i, j);
		}
	}
}
