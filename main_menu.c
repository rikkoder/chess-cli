#include <ncurses.h>
#include <wchar.h>

#include "main_menu.h"
#include "game.h"


static WINDOW *main_menu;
static int term_h, term_w;

void init_main_menu() {
	// storing values of window's (stdscr) size
	getmaxyx(stdscr, term_h, term_w);

	int key = -1;

	const int NO_OF_OPTS = 2;
	const int OPTS_SIZE = 12;

	// initialize options
	wchar_t woptions[NO_OF_OPTS][OPTS_SIZE+1];
	swprintf(woptions[0], OPTS_SIZE, L"%-10s\u265E", "Play");
	swprintf(woptions[1], OPTS_SIZE, L"%-11s", "Quit");
	int selected_opt = 0;

	// custom macro in game.h to create new win with parameters associated with window's name
	initialize_with_box(main_menu);
	wrefresh(stdscr);
	wrefresh(main_menu);

	while (true) {
		if (key == KEY_RESIZE) {
#ifdef PDCURSES
			resize_term(0, 0);
#endif
			getmaxyx(stdscr, term_h, term_w);
			translate_with_box(main_menu);
			wclear(stdscr);
		}

		if (key == KEY_UP || key == 'k')
			selected_opt = (selected_opt-1 + NO_OF_OPTS) % NO_OF_OPTS;
		else if (key == KEY_DOWN || key == 'j')
			selected_opt = (selected_opt+1) % NO_OF_OPTS;
		else if (key == '\n' || key == '\r' || key == KEY_ENTER) {
			if (selected_opt == 0) {
				// clear before displaying game
				wclear(main_menu);
				wrefresh(main_menu);

				init_game();

				// redrawing after end of game
				// size may have changed while playing game
				getmaxyx(stdscr, term_h, term_w);
				translate_with_box(main_menu);
				wclear(stdscr);
// 				wresize(main_menu, main_menu_h, main_menu_w);
// 				mvwin(main_menu, (window_h - main_menu_h)/2, (window_w - main_menu_w)/2);
// 				wclear(stdscr);
// 				wclear(main_menu);
// 				box(main_menu, 0, 0);
			}
			else {
				delwin(main_menu);
				return ;
			}
		}


		for (int i=0; i<NO_OF_OPTS; i++) {
			if (i == selected_opt)
				wattron(main_menu, A_STANDOUT);
			else
				wattroff(main_menu, A_STANDOUT);

			mvwaddnwstr(main_menu, main_menu_h/2 - NO_OF_OPTS/2 + i, main_menu_w/2 - (OPTS_SIZE-1)/2, woptions[i], OPTS_SIZE);
		}

		wrefresh(stdscr);
		wrefresh(main_menu);
		key = wgetch(main_menu);
	}

}
