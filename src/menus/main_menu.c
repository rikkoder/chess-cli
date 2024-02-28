#include <ncurses.h>
#include <wchar.h>
#include <string.h>
#include <stdlib.h>

#include "main_menu.h"
#include "../core/game.h"
#include "load_menu.h"


static	WINDOW	*main_menu_scr;
static	int		term_h;
static	int		term_w;


void init_main_menu() {
	// storing values of window's (stdscr) size
	getmaxyx(stdscr, term_h, term_w);

	int key = -1;

	const int NO_OF_OPTS = 4;
	const int OPTS_SIZE = 16;

	enum {PLAY_HUMAN_OPT, PLAY_AI_OPT, LOAD_OPT, QUIT_OPT};

	// initialize options
	wchar_t woptions[NO_OF_OPTS][OPTS_SIZE+1];
	swprintf(woptions[PLAY_HUMAN_OPT], OPTS_SIZE, L"%-*s\u265E", OPTS_SIZE-2, "Play vs Human");
	swprintf(woptions[PLAY_AI_OPT], OPTS_SIZE, L"%-*s\u265E", OPTS_SIZE-2, "Play vs AI");
	swprintf(woptions[LOAD_OPT], OPTS_SIZE, L"%-*s", OPTS_SIZE, "Load");
	swprintf(woptions[QUIT_OPT], OPTS_SIZE, L"%-*s", OPTS_SIZE, "Quit");
	int selected_opt = 0;

	// custom macro in src/utils/common.h to create new win with parameters associated with window's name
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
			if (selected_opt == PLAY_HUMAN_OPT) {
				// clear before displaying game
				wclear(main_menu_scr);
				wrefresh(main_menu_scr);

				// get game_settings from human_game_settings_menu
				game_settings_t game_settings;
				enum return_option_t return_option = init_human_game_settings_menu(&game_settings);

				if (return_option == OKAY) {
					enum game_return_code return_code;
					do {
						 return_code = init_game(game_settings);
					} while (return_code == RESTART);
				}

				// redrawing after end of game
				// size may have changed while playing game
				getmaxyx(stdscr, term_h, term_w);
				translate_with_box(main_menu_scr);
				wclear(stdscr);
			} else if (selected_opt == PLAY_AI_OPT) {
				// clear before displaying game
				wclear(main_menu_scr);
				wrefresh(main_menu_scr);

				// get game_settings from ai_game_settings_menu
				game_settings_t game_settings;
				enum return_option_t return_option = init_ai_game_settings_menu(&game_settings);

				if (return_option == OKAY) {
					enum game_return_code return_code;
					do {
						 return_code = init_game(game_settings);
					} while (return_code == RESTART);
				}

				// redrawing after end of game
				// size may have changed while playing game
				getmaxyx(stdscr, term_h, term_w);
				translate_with_box(main_menu_scr);
				wclear(stdscr);
			} else if (selected_opt == LOAD_OPT) {
				game_settings_t game_settings = {
					.game_mode = LOAD_MODE
				};
// 				timestamp_t load_timestamp;
				memset(game_settings.load_timestamp, 0, sizeof(game_settings.load_timestamp));
				if (load_menu(game_settings.load_timestamp) == LOAD_FILE_SELECTED) {
					enum game_return_code return_code = init_game(game_settings);
					if (return_code == INVALID_LOAD) {
						show_warning_scr("Invalid Load File !");
					}
					while (return_code == RESTART) {
						game_settings_t game_settings = {
							.game_mode = AI_MODE
						};
						return_code = init_game(game_settings);
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
