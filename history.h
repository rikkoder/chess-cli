#ifndef HISTORY_H
#define HISTORY_H

#include "board.h"

#define MAX_MOVE_NOTATION_SIZE	7
#define TIMESTAMP_SIZE			14


typedef	char				timestamp_t[TIMESTAMP_SIZE+1];
typedef	struct history_t	history_t;

history_t*			create_history	(void);
void				add_move		(history_t *history, const board_t *board, const char *const move_notation);
void				undo			(history_t *history);
void				go_back			(history_t *history, int n);
void				delete_history	(history_t *history);
const board_t*		peek_board		(const history_t *history, int n);
int					get_size		(const history_t *history);
const char *const	peek_move		(const history_t *history, int n);
const char *const	get_timestamp	(const history_t *history);
history_t*			reverse_history	(const history_t *history);


#endif
