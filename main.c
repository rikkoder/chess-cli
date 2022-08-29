#include <stdio.h>
#include <stdlib.h>
#include <ncurses.h>
#include <locale.h>

#include "main_menu.h"

const long double SCR_RES = (1920./1080);

int main (int argc, char **argv) {
	setlocale(LC_ALL, "");	// support printing of UNICODE chars
	initscr();
	noecho();
	curs_set(FALSE);
	keypad(stdscr, TRUE);

// 	WINDOW *main_menu_win = NULL;
	init_main_menu();

// 	delwin(main_menu_win);
	endwin();

	return EXIT_SUCCESS;
}
