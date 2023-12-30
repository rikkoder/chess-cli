#ifndef UTILITIES_H
#define UTILITIES_H

#include "history.h"

#define swap(x, y, type)	{type t = x; x = y; y = t;}
#define min(x, y)			((x) < (y) ? (x) : (y))

#define FILE_EXT		"hstk"
#define FILE_DIR		".chess-cli-files"
//#define FILENAME_SIZE	(TIMESTAMP_SIZE + 5)

extern	char*	save_directory;
extern	int		save_directory_size;

char*		itoa		(int i, char *a);

// load and save functions
void		save_hstk	(const history_t *history);
history_t*	load_hstk	(const timestamp_t timestamp);
void		export_pgn	(const history_t *history);


#endif
