#include "main_menu.h"
#include "game.h"

extern const long double SCR_RES;

static WINDOW *main_menu;
static int term_h, term_w;

void init_main_menu() {
// 	int term_w, term_h, main_menu_w = 20, main_menu_h = 10;

	// storing values of window's (stdscr) size
	getmaxyx(stdscr, term_h, term_w);
// 	main_menu_w = SCR_RES * main_menu_h;

	int key = -1;

	const int NO_OF_OPTS = 2;
	const int OPTS_SIZE = 12;

	// initialize options
	wchar_t woptions[NO_OF_OPTS][OPTS_SIZE+1];
	swprintf(woptions[0], OPTS_SIZE, L"%-10s\u265E", "Play");
	swprintf(woptions[1], OPTS_SIZE, L"%-11s", "Quit");
	int selected_opt = 0;

// 	WINDOW *main_menu = newwin(main_menu_h, main_menu_w, (term_h - main_menu_h)/2, (term_w - main_menu_w)/2);
// 	keypad(main_menu, TRUE);
	// custom macro in game.h to create new win with parameters associated with window's name
	initialize_with_macro(main_menu);
	box(main_menu, 0, 0);
	wrefresh(stdscr);
	wrefresh(main_menu);

	while (TRUE) {
		if (key == KEY_RESIZE) {
#ifdef PDCURSES
			resize_term(0, 0);
#endif
			getmaxyx(stdscr, term_h, term_w);
// 			main_menu_w = SCR_RES * main_menu_h;

			wresize(main_menu, main_menu_h, main_menu_w);
			mvwin(main_menu, (term_h - main_menu_h)/2, (term_w - main_menu_w)/2);
			wclear(main_menu);
			wclear(stdscr);
			box(main_menu, 0, 0);
		}

		if (key == KEY_UP || key == 'k') selected_opt = (selected_opt+1)%NO_OF_OPTS;
		else if (key == KEY_DOWN || key == 'j') selected_opt = (selected_opt-1 + NO_OF_OPTS)%NO_OF_OPTS;
		else if (key == '\n' || key == '\r' || key == KEY_ENTER) {
			if (selected_opt == 0) {
				// clear before displaying game
				wclear(main_menu);
				wrefresh(main_menu);

				init_game();

				// redrawing after end of game
				// size may have changed while playing game
				getmaxyx(stdscr, term_h, term_w);
				wresize(main_menu, main_menu_h, main_menu_w);
				mvwin(main_menu, (term_h - main_menu_h)/2, (term_w - main_menu_w)/2);
				wclear(stdscr);
				wclear(main_menu);
				box(main_menu, 0, 0);
			}
			else {
				delwin(main_menu);
				return ;
			}
		}


		// native and ncurses resolutions
// 		double res[2] = {1920./1080, (float)term_w/term_h};
		for (int i=0; i<NO_OF_OPTS; i++) {
			if (i == selected_opt) wattron(main_menu, A_STANDOUT);
			else wattroff(main_menu, A_STANDOUT);

			mvwaddnwstr(main_menu, main_menu_h/2 - NO_OF_OPTS/2 + i, main_menu_w/2 - (OPTS_SIZE-1)/2, woptions[i], OPTS_SIZE);

			// print native and ncurses resolutions
// 			mvwprintw(main_menu, main_menu_h/2 - NO_OF_OPTS/2 + i, main_menu_w/2 - (OPTS_SIZE-1)/2, "%.6f", res[i]);
		}

		wrefresh(stdscr);
		wrefresh(main_menu);
		key = wgetch(main_menu);
	}

}
