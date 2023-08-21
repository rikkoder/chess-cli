#ifndef HISTORY_H
#define HISTORY_H

#include "board.h"

typedef struct history_t history_t;

history_t*		create_history	(void);
void			add_move		(history_t *history, const board_t *board);
void			undo			(history_t *history);
void			go_back			(history_t *history, int n);
void			delete_history	(history_t *history);
const board_t*	peek			(const history_t *history, int n);
int				get_size		(const history_t *history);


#endif
