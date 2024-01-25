#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <stdio.h>

#include "history.h"
#include "chess_engine.h"

typedef struct board_node_t board_node_t;

struct history_t {
	board_node_t *top;
	int size;
	timestamp_t timestamp;
	player_t players[2];
	enum result result;
};

struct board_node_t {
	board_node_t *prev;
	board_t *board;
	char move_notation[MAX_MOVE_NOTATION_SIZE+1];
};

static	const board_node_t*		peek	(const history_t *history, int n);


history_t* create_history (const player_t plr1, const player_t plr2) {
	history_t *history = (history_t*) malloc(sizeof(history_t));
	memset(history, 0, sizeof(history_t));

	history->top = NULL;
	history->size = 0;

	time_t t = time(NULL);
	struct tm tm = *localtime(&t);
	snprintf(history->timestamp, TIMESTAMP_SIZE + 1, "%04d%02d%02d%02d%02d%02d", tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec);

	history->players[0] = plr1;
	history->players[1] = plr2;
	history->result = PENDING;

	return history;
}


void add_move (history_t *history, const board_t *board, const char *const move_notation) {
	board_node_t *board_node = (board_node_t*) malloc(sizeof(board_node_t));
	memset(board_node, 0, sizeof(board_node_t));

	board_node->prev = history->top;
	board_node->board = (board_t*) malloc(sizeof(board_t));
	memset(board_node->board, 0, sizeof(board_t));
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


void set_timestamp (history_t *history, const timestamp_t timestamp) {
	strncpy(history->timestamp, timestamp, TIMESTAMP_SIZE);
}


history_t* reverse_history (const history_t *history) {
	player_t plr1, plr2;
	get_players(history, &plr1, &plr2);
	history_t *reversed_history = create_history(plr1, plr2);
	if (reversed_history == NULL)
		return NULL;
	strncpy(reversed_history->timestamp, history->timestamp, TIMESTAMP_SIZE);
	board_node_t *cur = history->top;
	while (cur != NULL) {
		add_move(reversed_history, cur->board, cur->move_notation);
		cur = cur->prev;
	}
	return reversed_history;
}


void update_result (history_t *history) {
	if (history == NULL || get_size(history) == 0)
		return;
	is_game_finished(history->top->board, history);
	history->result = history->top->board->result;
}


enum result get_result (const history_t *history) {
	return history->result;
}


void get_players (const history_t *history, player_t *plr1, player_t *plr2) {
	if (history == NULL)
		return;
	*plr1 = history->players[0];
	*plr2 = history->players[1];
}


void init_player (player_t *plr, const char *const name, const enum player_type type) {
	strncpy(plr->name, name, sizeof(player_name_t));
	plr->type = type;
}


static const board_node_t* peek (const history_t *history, int n) {
	if (n >= history->size || n < 0)
		return NULL;

	const board_node_t *curr = history->top;
	while (n--)
		curr = curr->prev;

	return curr;
}
