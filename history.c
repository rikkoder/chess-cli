#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <stdio.h>

#include "history.h"

typedef struct board_node_t board_node_t;

struct history_t {
	board_node_t *top;
	int size;
	char timestamp[TIMESTAMP_SIZE+1];
};

struct board_node_t {
	board_node_t *prev;
	board_t *board;
	char move_notation[MAX_MOVE_NOTATION_SIZE+1];
};

static	const board_node_t*		peek	(const history_t *history, int n);


history_t* create_history (void) {
	history_t *history = (history_t*) malloc(sizeof(history_t));
	memset(history, 0, sizeof(history_t));

	history->top = NULL;
	history->size = 0;

	time_t t = time(NULL);
	struct tm tm = *localtime(&t);
	snprintf(history->timestamp, TIMESTAMP_SIZE + 1, "%04d%02d%02d%02d%02d%02d\0", tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec);

	return history;
}


void add_move (history_t *history, const board_t *board, const char *const move_notation) {
	board_node_t *board_node = (board_node_t*) malloc(sizeof(board_node_t));
	memset(board_node, 0, sizeof(board_node_t));

	board_node->prev = history->top;
	board_node->board = (board_t*) malloc(sizeof(board_t));
	copy_board(board_node->board, board);
	strncpy(board_node->move_notation, move_notation, MAX_MOVE_NOTATION_SIZE);

	history->top = board_node;
	history->size++;
}


void undo (history_t *history) {
	board_node_t *top = history->top;
	if (top == NULL)
		return;

	history->top = top->prev;
	delete_board(top->board);
	free(top);
	history->size--;
}


void go_back (history_t *history, int n) {
	if (n > history->size || n <= 0)
		return;

	while (n--)
		undo(history);
}


void delete_history (history_t *history) {
	if (history == NULL)
		return;

	go_back(history, history->size);
	free(history);
}


const board_t* peek_board (const history_t *history, int n) {
	const board_node_t *curr = peek(history, n);
	return (curr != NULL ? curr->board: NULL);
// 	if (n > history->size || n <= 0)
// 		return NULL;
// 
// 	const board_node_t *curr = history->top;
// 	while (--n)
// 		curr = curr->prev;
// 
// 	return curr->board;
}


int get_size (const history_t *history) {
	return history->size;
}


const char *const peek_move (const history_t *history, int n) {
	const board_node_t *curr = peek(history, n);
	return (curr != NULL ? curr->move_notation: "\0");
}


const char *const get_timestamp(const history_t *history) {
	return history->timestamp;
}


static const board_node_t* peek (const history_t *history, int n) {
	if (n >= history->size || n < 0)
		return NULL;

	const board_node_t *curr = history->top;
	while (n--)
		curr = curr->prev;

	return curr;
}
