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
 *	MOVE_NOTATION	-	FORMAT DEFINED BY chess_engine.c:find_move_notation					(DYNAMIC SIZE)
 *	LVL2_DELIMITER	-	A SINGLE CHARACTER TO MARK END OF MOVE_NOTATION						(1 CHAR)
 *	CAPTURED_PIECES	-	2 digits for each pieces except kings in order as in board.c:PIECES	(20 CHARS)
 *	BOARD_STATE		-	FILLED FROM (0, 0) CELL TO (7, 7) CELL
 *					-	ASCII_PIECE_FACE PIECE_MOVED/NOT_MOVED								(2 CHARS FOR EACH FILLED CELL)
 *					-	INTEGER REPRESENTING COUNT OF CONTINUOUS EMPTY CELLS				(DYNAMIC SIZE)
 *	LVL1_DELIMITER	-	A SINGLE CHARACTER TO MARK END OF BOARD_STATE						(1 CHAR)
 */

	if (history == NULL || get_size(history) == 0)
		return;

	int filename_size = save_directory_size + TIMESTAMP_SIZE + strlen(FILE_EXT) + 1;
	char *filename = (char *) malloc(sizeof(char) * filename_size);
	snprintf(filename, filename_size, "%s%s.%s", save_directory, get_timestamp(history), FILE_EXT);
	FILE *fp = fopen(filename, "w");

	const int BUFFER_SIZE = 1024;
	char buffer[BUFFER_SIZE+1];
	int ptr = 0;

	const int CAPTURED_DATA_SIZE = 20;	// fixed lenght, 10 capturable pieces, 3 characters for each
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


history_t* load_hstk (const timestamp_t timestamp) {
	if (timestamp == NULL)
		return NULL;

	int filename_size = save_directory_size + TIMESTAMP_SIZE + strlen(FILE_EXT) + 1;
	char *filename = (char *) malloc(sizeof(char) * filename_size);
	snprintf(filename, filename_size, "%s%s.%s", save_directory, timestamp, FILE_EXT);
	FILE *fp = fopen(filename, "r");
	if (fp == NULL)
		return NULL;

	history_t *history = create_history();
	set_timestamp(history, timestamp);

	bool error = false;
	board_t *board = NULL;
	piece_t *piece = NULL;
	while (!error) {
		char move_notation[MAX_MOVE_NOTATION_SIZE+1];
		memset(move_notation, 0, MAX_MOVE_NOTATION_SIZE+1);
		char c;
		unsigned int ptr = 0;
		while ((c = getc(fp)) != EOF && c != LVL2_DELIMITER) {
			move_notation[ptr++] = c;
		}
		if (c == EOF) {
			break;
		}

		board = calloc(1, sizeof(board_t));
		if (get_size(history)&1)
			board->chance = WHITE;
		else
			board->chance = BLACK;
		board->result = PENDING;
		// board->captured
		for (int i = 0; i < 2 && !error; i++) {
			for (int j = 0; j < PIECE_TYPES && !error; j++) {
				if ((1 << j) & KING) {
					board->captured[i][j] = 0;
					continue;
				}
				int d = 10;
				while (d != 0 && (c = getc(fp)) >= '0' && c <= '9') {
					board->captured[i][j] += (c-'0')*d;
					d /= 10;
				}
				if (c < '0' || c > '9') {
					error = true;
					break;
				}
			}
		}
		// board->tiles
		short row = 0, col = 0;
		piece = NULL;
		unsigned int empty_cells_count = 0;
		while ((c = getc(fp)) != LVL1_DELIMITER && !error) {
			if (c >= '0' && c <= '9') {
				if (piece != NULL) {
					error = true;
					break;
				}

				empty_cells_count = (empty_cells_count * 10) + (c - '0');
				continue;
			}

			while (empty_cells_count > 0) {
				board->tiles[row][col].row = row;
				board->tiles[row][col].col = col;
				if (col == 7) {
					col = 0;
					if (row < 7) row++;
					else {
						error = true;
						break;
					}
				} else {
					col++;
				}
				empty_cells_count--;
			}

			bool is_piece = false;
			for (int i = 0; i < 2 && !is_piece; i++) {
				for (int j = 0; j < PIECE_TYPES && !is_piece; j++) {
					if (c == PIECES[0][i][j]) {
						piece = (piece_t *) calloc(1, sizeof(piece_t));
						if (i != WHITE) piece->face |= BLACK;
						piece->face |= (1 << j);
						is_piece = true;
						
						if (row > 7 || col > 7) {
							error = true;
							break;
						}
						if ((1 << j) & KING)
							board->kings[i] = &(board->tiles[row][col]);
					}
				}
			}
			if (is_piece) {
				continue;
			}

			if (c == PIECE_MOVED || c == PIECE_NOT_MOVED) {
				if (piece == NULL || row > 7 || col > 7) {
					error = true;
					break;
				}
				piece->is_moved = (c == PIECE_MOVED ? true: false);
				board->tiles[row][col].piece = piece;
				piece = NULL;
				board->tiles[row][col].row = row;
				board->tiles[row][col].col = col;

				if (col == 7) {
					col = 0;
					row++;
				} else {
					col++;
				}
			} else {
				error = true;
				break;
			}
		}

		while (empty_cells_count > 0) {
			board->tiles[row][col].row = row;
			board->tiles[row][col].col = col;
			if (col == 7) {
				col = 0;
				if (row < 7) row++;
				else {
					error = true;
					break;
				}
			} else {
				col++;
			}
			empty_cells_count--;
		}

		add_move(history, board, move_notation);
		delete_board(board);
		board = NULL;
		memset(move_notation, 0, MAX_MOVE_NOTATION_SIZE+1);
	}

	fclose(fp);
	if (error) {
		if (board) free(board);
		if (piece) free(piece);
		fprintf(stderr, "error while loading file... INVALID SAVE FILE");
		delete_history(history);
		exit(EXIT_FAILURE);
	}

	return history;
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
