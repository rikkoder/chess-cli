#ifndef UTILITIES_H
#define UTILITIES_H

#include "history.h"

#define swap(x, y, type)	{type t = x; x = y; y = t;}
#define min(x, y)			((x) < (y) ? (x) : (y))

//#define FILENAME_SIZE	(TIMESTAMP_SIZE + 5)

extern	char*	save_directory;
extern	int		save_directory_size;
extern	char*	pgn_directory;;
extern	int		pgn_directory_size;

char*		itoa		(int i, char *a);
bool		save_hstk	(const history_t *history);
history_t*	load_hstk	(const timestamp_t timestamp);
bool		export_pgn	(const history_t *history);


#endif
