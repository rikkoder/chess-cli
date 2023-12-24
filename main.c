#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ncurses.h>
#include <locale.h>

#include "main.h"
#include "main_menu.h"
#include "utilities.h"


char	*save_directory		=	NULL;
int		save_directory_size	=	0;	

int main (int argc, char **argv) {
	// set save_directory and save_directory_size
	char *home_dir = getenv("HOME");
	if (home_dir == NULL) {
		fprintf(stderr, "HOME env var not found!!\n");
		exit(EXIT_FAILURE);
	}

	save_directory_size = strlen(home_dir) + 1 + strlen(FILE_DIR) + 1 + 1;
	save_directory = (char *) malloc(save_directory_size * sizeof(char));
	if (save_directory == NULL) {
		fprintf(stderr, "couldn't allocate memory");
		exit(EXIT_FAILURE);
	}
	memset(save_directory, 0, save_directory_size * sizeof(char));
	snprintf(save_directory, save_directory_size, "%s/%s/", home_dir, FILE_DIR);


	setlocale(LC_ALL, "");	// support printing of UNICODE chars
	initscr();
	noecho();
	curs_set(false);
	keypad(stdscr, true);

	init_main_menu();

	endwin();

	return EXIT_SUCCESS;
}
