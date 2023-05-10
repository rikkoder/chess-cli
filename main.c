#include <stdio.h>
#include <stdlib.h>
#include <ncurses.h>
#include <locale.h>

#include "main.h"
#include "main_menu.h"


int main (int argc, char **argv) {
	setlocale(LC_ALL, "");	// support printing of UNICODE chars
	initscr();
	noecho();
	curs_set(false);
	keypad(stdscr, true);

	init_main_menu();

	endwin();

	return EXIT_SUCCESS;
}
