#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "utilities.h"
#include "history.h"


static	const char	LVL1_DELIMITER	=	'^';
static	const char	LVL2_DELIMITER	=	'@';
static	const char	PIECE_MOVED		=	'X';
static	const char	PIECE_NOT_MOVED	=	'O';

static	void		write_to_file	(FILE *fp, const char buffer[], const int ptr);


char* itoa(int i, char *a) {
	if (i == 0) {
		a[0] = '0';
		a[1] = '\0';
		return a;
	}
	int off = 0;
	if (i < 0) {
		off = 1;
		a[0] = '-';
	}
	int d = off;
	while (i) {
		a[d++] = (i%10)+'0';
		i /= 10;
	}
	a[d] = '\0';
	int s = off, e = d-1;
	while (s < e) {
		swap(a[s], a[e], char);
		s++; e--;
	}
	return a;
}


void save_hstk (const history_t *history) {
/*
 *	FORMAT OF SAVE FILE
 *
 *	MOVE_NOTATION	LVL2_DELIMITER	CAPTURED_PIECES(FIXED SIZE)	BOARD_STATE	LVL1DELIMITER
 *
 *	MOVE_NOTATION	-	FORMAT DEFINED BY chess_engine.c:find_move_notation		(DYNAMIC SIZE)
 *	LVL2_DELIMITER	-	A SINGLE CHARACTER TO MARK END OF MOVE_NOTATION			(1 CHAR)
 *	CAPTURED_PIECES	-	Q00R00B00N00P00q00r00b00n00p00							(30 CHARS)
 *	BOARD_STATE		-	FILLED FROM (0, 0) CELL TO (7, 7) CELL
 *					-	ASCII_PIECE_FACE PIECE_MOVED/NOT_MOVED					(2 CHARS FOR EACH FILLED CELL)
 *					-	INTEGER REPRESENTING COUNT OF CONTINUOUS EMPTY CELLS	(DYNAMIC SIZE)
 *	LVL1_DELIMITER	-	A SINGLE CHARACTER TO MARK END OF BOARD_STATE			(1 CHAR)
 */

	int filename_size = save_directory_size + TIMESTAMP_SIZE + strlen(FILE_EXT) + 1;
	char *filename = (char *) malloc(sizeof(char) * filename_size);
	snprintf(filename, filename_size, "%s%s.%s", save_directory, get_timestamp(history), FILE_EXT);
	FILE *fp = fopen(filename, "w");

	const int BUFFER_SIZE = 1024;
	char buffer[BUFFER_SIZE+1];
	int ptr = 0;

	const int CAPTURED_DATA_SIZE = 30;	// fixed lenght, 10 capturable pieces, 3 characters for each
	const int MAX_BOARD_DATA_SIZE = 128;	// if each of 64 cell is filled, 2 characters for each piece
	const int MAX_DATA_SIZE = MAX_MOVE_NOTATION_SIZE + 1 + CAPTURED_DATA_SIZE + MAX_BOARD_DATA_SIZE + 1;	// delimiters after variable sized move notation and board description.
	history_t *reversed_history = reverse_history(history);
	int total_moves = get_size(reversed_history);
	for (int i=0; i<total_moves; i++) {
		// prevent buffer overflow
		if (ptr + MAX_DATA_SIZE > BUFFER_SIZE) {
			// write into file till ptr
			write_to_file(fp, buffer, ptr);
			ptr = 0;
		}

		// store move notation
		const char *move = peek_move(reversed_history, 0);
		for (int j = 0; j < MAX_MOVE_NOTATION_SIZE && move[j] != '\0'; j++)
			buffer[ptr++] = move[j];
		buffer[ptr++] = LVL2_DELIMITER;
		
		const board_t *board = peek_board(reversed_history, 0);
		// store #captured pieces data
		for (int j = 0; j < 2 ; j++) {
			for (int k = 0; k < PIECE_TYPES; k++) {
				if ((1 << k) & KING) continue;
				buffer[ptr++] = PIECES[ASCII][j][k];
				int c = board->captured[j][k];
				buffer[ptr+1] = '0' + c%10;
				c /= 10;
				buffer[ptr] = '0' + c%10;
				ptr += 2;
			}
		}

		// store board description
		int empty_cells_count = 0;
		for (int j = 0; j < 8; j++) {
			for (int k = 0; k < 8; k++) {
				const piece_t *piece = board->tiles[j][k].piece;
				if (piece == NULL) {
					empty_cells_count++;
					continue;
				}
				if (empty_cells_count > 0) {
					char cnt[3];
					itoa(empty_cells_count, cnt);
					buffer[ptr++] = cnt[0];
					if (cnt[1] != '\0') buffer[ptr++] = cnt[1];
					empty_cells_count = 0;
				}
				buffer[ptr++] = get_piece_for_move_notation(piece);
				if (piece->is_moved)
					buffer[ptr++] = PIECE_MOVED;
				else
					buffer[ptr++] = PIECE_NOT_MOVED;
			}
		}
		if (empty_cells_count > 0) {
			char cnt[3];
			itoa(empty_cells_count, cnt);
			buffer[ptr++] = cnt[0];
			if (cnt[1] != '\0') buffer[ptr++] = cnt[1];
			empty_cells_count = 0;
		}
		buffer[ptr++] = LVL1_DELIMITER;

		// remove top board
		undo(reversed_history);
	}

	delete_history(reversed_history);

	// write into file till ptr
	write_to_file(fp, buffer, ptr);
	fclose(fp);
}


history_t* load_hstk (const char *filename) {

}


static void write_to_file (FILE *fp, const char buffer[], const int ptr) {
	int ret_code = 0;
	for (int p = 0; (ret_code != EOF) && p < ptr; p++) {
		ret_code = putc(buffer[p], fp);
	}

	if (ret_code == EOF && ferror(fp)) {
		fprintf(stderr, "failed saving game... couldn't write to file!!\n");
		exit(EXIT_FAILURE);
	}
}
