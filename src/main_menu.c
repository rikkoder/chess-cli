#include <ncurses.h>
#include <wchar.h>
#include <string.h>
#include <stdlib.h>

#include "main_menu.h"
#include "game.h"
#include "utilities.h"
#include "load_menu.h"


static	WINDOW	*main_menu_scr;
static	int		term_h;
static	int		term_w;


void init_main_menu() {
	// storing values of window's (stdscr) size
	getmaxyx(stdscr, term_h, term_w);

	int key = -1;

	const int NO_OF_OPTS = 3;
	const int OPTS_SIZE = 12;

	enum {PLAY_OPT, LOAD_OPT, QUIT_OPT};

	// initialize options
	wchar_t woptions[NO_OF_OPTS][OPTS_SIZE+1];
	swprintf(woptions[0], OPTS_SIZE, L"%-10s\u265E", "Play");
	swprintf(woptions[1], OPTS_SIZE, L"%-11s", "Load");
	swprintf(woptions[2], OPTS_SIZE, L"%-11s", "Quit");
	int selected_opt = 0;

	// custom macro in main.h to create new win with parameters associated with window's name
	initialize_with_box(main_menu_scr);
	wrefresh(stdscr);
	wrefresh(main_menu_scr);

	while (true) {
		box(main_menu_scr, 0, 0);

		if (key == KEY_RESIZE) {
			getmaxyx(stdscr, term_h, term_w);
			translate_with_box(main_menu_scr);
			wclear(stdscr);
		}

		if (key == KEY_UP || key == 'k')
			selected_opt = (selected_opt-1 + NO_OF_OPTS) % NO_OF_OPTS;
		else if (key == KEY_DOWN || key == 'j')
			selected_opt = (selected_opt+1) % NO_OF_OPTS;
		else if (key == '\n' || key == '\r' || key == KEY_ENTER) {
			if (selected_opt == PLAY_OPT) {
				// clear before displaying game
				wclear(main_menu_scr);
				wrefresh(main_menu_scr);

				enum game_return_code return_code;
				do {
					 return_code = init_game(NULL);
				} while (return_code == RESTART);

				// redrawing after end of game
				// size may have changed while playing game
				getmaxyx(stdscr, term_h, term_w);
				translate_with_box(main_menu_scr);
				wclear(stdscr);
			} else if (selected_opt == LOAD_OPT) {
				timestamp_t load_timestamp;
				memset(load_timestamp, 0, sizeof(load_timestamp));
				if (load_menu(load_timestamp) == LOAD_FILE_SELECTED) {
					enum game_return_code return_code = init_game(load_timestamp);
					if (return_code == INVALID_LOAD) {
						show_warning_scr("Invalid Load File !");
					}
					while (return_code == RESTART) {
						return_code = init_game(NULL);
					}
				};
			} else {
				delwin(main_menu_scr);
				return ;
			}
		}


		for (int i=0; i<NO_OF_OPTS; i++) {
			if (i == selected_opt)
				wattron(main_menu_scr, A_STANDOUT);
			mvwaddnwstr(main_menu_scr, main_menu_scr_h/2 - NO_OF_OPTS/2 + i, main_menu_scr_w/2 - (OPTS_SIZE-1)/2, woptions[i], OPTS_SIZE);
			wattroff(main_menu_scr, A_STANDOUT);
		}

		wrefresh(stdscr);
		wrefresh(main_menu_scr);
		key = wgetch(main_menu_scr);
	}

	delwin(main_menu_scr);
}
