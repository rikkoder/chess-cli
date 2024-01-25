#ifndef FILE_H
#define FILE_H

#include "../core/history.h"


extern	char*	save_directory;
extern	int		save_directory_size;
extern	char*	pgn_directory;;
extern	int		pgn_directory_size;

bool		save_hstk	(const history_t *history);
history_t*	load_hstk	(const timestamp_t timestamp);
bool		export_pgn	(const history_t *history);


#endif
