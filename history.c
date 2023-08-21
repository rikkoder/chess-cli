#include <stdlib.h>
#include <string.h>

#include "history.h"

typedef struct board_node_t board_node_t;

struct history_t {
	board_node_t *top;
	int size;
};

struct board_node_t {
	board_node_t *prev;
	board_t *board;
};


history_t* create_history (void) {
	history_t *history = (history_t*) malloc(sizeof(history_t));
	memset(history, 0, sizeof(history_t));

	history->top = NULL;
	history->size = 0;

	return history;
}


void add_move (history_t *history, const board_t *board) {
	board_node_t *board_node = (board_node_t*) malloc(sizeof(board_node_t));
	memset(board_node, 0, sizeof(board_node_t));

	board_node->prev = history->top;
	board_node->board = (board_t*) malloc(sizeof(board_t));
	copy_board(board_node->board, board);

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


const board_t* peek (const history_t *history, int n) {
	if (n > history->size || n <= 0)
		return NULL;

	const board_node_t *curr = history->top;
	while (--n)
		curr = curr->prev;

	return curr->board;
}


int get_size (const history_t *history) {
	return history->size;
}
