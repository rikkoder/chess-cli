#include <ncurses.h>
#include <string.h>
#include <stdlib.h>

#include "game_settings_menu.h"

static	WINDOW	*game_settings_menu_scr;
static	int		term_h;
static	int		term_w;

static	int	get_option_offset	(const char *const option_str);


enum return_option_t init_human_game_settings_menu (game_settings_t *game_settings) {
	player_name_t plr1_name, plr2_name;
	int plr1_name_len = 0, plr2_name_len = 0;
	int timer_opts[] = { 0, 3, 5, 10, 15, 30, 60 };	// in mins, 0 is off
	int selected_timer = 0;
	int no_of_timers = sizeof(timer_opts)/sizeof(timer_opts[0]);

	// storing values of window's (stdscr) size
	getmaxyx(stdscr, term_h, term_w);

	int key = -1;

	const int NO_OF_OPTS = 5;
	const int OPTS_SIZE = 7 + PLAYERNAME_SIZE;

	enum {PLR1_OPT, PLR2_OPT, TIMER_OPT, START_OPT, CANCEL_OPT};

	// initialize options
	char options[NO_OF_OPTS][OPTS_SIZE+1];
	snprintf(options[PLR1_OPT], OPTS_SIZE, "%-*s", OPTS_SIZE, "P1: ");
	snprintf(options[PLR2_OPT], OPTS_SIZE, "%-*s", OPTS_SIZE, "P2: ");
	snprintf(options[TIMER_OPT], OPTS_SIZE, "%-*s", OPTS_SIZE, "Timer: ");
	// separate layout for these two options
	snprintf(options[START_OPT], OPTS_SIZE, "%-*s", OPTS_SIZE/2, "Start");
	snprintf(options[CANCEL_OPT], OPTS_SIZE, "%-*s", OPTS_SIZE/2, "Cancel");
	int selected_opt = 0;

	// custom macro in src/utils/common.h to create new win with parameters associated with window's name
	initialize_with_box(game_settings_menu_scr);
	wrefresh(stdscr);
	wrefresh(game_settings_menu_scr);

	while (true) {
		box(game_settings_menu_scr, 0, 0);

		if (key == KEY_RESIZE) {
			getmaxyx(stdscr, term_h, term_w);
			translate_with_box(game_settings_menu_scr);
			wclear(stdscr);
		}

		// handle name input
		if (selected_opt == PLR1_OPT) {
			if ((plr1_name_len < PLAYERNAME_SIZE) && ((key >= 'a' && key <= 'z') || (key >= 'A' && key <= 'Z') || (key >= '0' && key <= '9') || key == '_')) {
				plr1_name[plr1_name_len++] = key;
			} else if (plr1_name_len > 0 && key == KEY_BACKSPACE) {
				plr1_name_len--;
				wclear(game_settings_menu_scr);
				box(game_settings_menu_scr, 0, 0);
			}
		} else if (selected_opt == PLR2_OPT) {
			if ((plr2_name_len < PLAYERNAME_SIZE) && ((key >= 'a' && key <= 'z') || (key >= 'A' && key <= 'Z') || (key >= '0' && key <= '9') || key == '_')) {
				plr2_name[plr2_name_len++] = key;
			} else if (plr2_name_len > 0 && key == KEY_BACKSPACE) {
				plr2_name_len--;
				wclear(game_settings_menu_scr);
				box(game_settings_menu_scr, 0, 0);
			}
		}

		// handle timer input
		if (selected_opt == TIMER_OPT) {
			if (key == KEY_LEFT || key == 'h')
				selected_timer = (selected_timer - 1 + no_of_timers) % no_of_timers;
			else if (key == KEY_RIGHT || key == 'l')
				selected_timer = (selected_timer + 1) % no_of_timers;
		}

		// handle menu navigation (don't allow cycling as layout isn't continuous)
		if (key == KEY_UP || (selected_opt != PLR1_OPT && selected_opt != PLR2_OPT && key == 'k'))
			selected_opt = max(selected_opt-1, 0);
		else if (key == KEY_DOWN || (selected_opt != PLR1_OPT && selected_opt != PLR2_OPT && key == 'j'))
			selected_opt = min(selected_opt+1, NO_OF_OPTS-1);
		else if (key == '\n' || key == '\r' || key == KEY_ENTER) {
			if (selected_opt == START_OPT) {
				plr1_name[plr1_name_len] = '\0';
				plr2_name[plr2_name_len] = '\0';
				init_player(game_settings->players, plr1_name, HUMAN);
				init_player(game_settings->players+1, plr2_name, HUMAN);
				game_settings->game_mode = HUMAN_MODE;
				game_settings->time_limit = timer_opts[selected_timer];

				delwin(game_settings_menu_scr);
				return OKAY;
			} else if (selected_opt == CANCEL_OPT) {
				delwin(game_settings_menu_scr);
				return CANCEL;
			} else {
				selected_opt = min(selected_opt+1, NO_OF_OPTS-1);
			}
		}


		// layout for options before START button
		for (int i=0; i<START_OPT; i++) {
			if (i == selected_opt)
				wattron(game_settings_menu_scr, A_STANDOUT);
			mvwaddnstr(game_settings_menu_scr, game_settings_menu_scr_h/2 - NO_OF_OPTS/2 + i, game_settings_menu_scr_w/2 - (OPTS_SIZE-1)/2, options[i], OPTS_SIZE);

			wattroff(game_settings_menu_scr, A_STANDOUT);
		}

		// display player names
		if (PLR1_OPT == selected_opt)
			wattron(game_settings_menu_scr, A_STANDOUT);
		mvwaddnstr(game_settings_menu_scr, game_settings_menu_scr_h/2 - NO_OF_OPTS/2 + PLR1_OPT, game_settings_menu_scr_w/2 - (OPTS_SIZE-1)/2 + get_option_offset(options[PLR1_OPT]) + 1, plr1_name, plr1_name_len);
		if (plr1_name_len < PLAYERNAME_SIZE && PLR1_OPT == selected_opt) {
			wattron(game_settings_menu_scr, A_BLINK);
			mvwaddch(game_settings_menu_scr, game_settings_menu_scr_h/2 - NO_OF_OPTS/2 + PLR1_OPT, game_settings_menu_scr_w/2 - (OPTS_SIZE-1)/2 + get_option_offset(options[PLR1_OPT]) + 1 + plr1_name_len, '_');
			wattroff(game_settings_menu_scr, A_BLINK);
		}
		wattroff(game_settings_menu_scr, A_STANDOUT);
		if (PLR2_OPT == selected_opt)
			wattron(game_settings_menu_scr, A_STANDOUT);
		mvwaddnstr(game_settings_menu_scr, game_settings_menu_scr_h/2 - NO_OF_OPTS/2 + PLR2_OPT, game_settings_menu_scr_w/2 - (OPTS_SIZE-1)/2 + get_option_offset(options[PLR2_OPT]) + 1, plr2_name, plr2_name_len);
		if (plr2_name_len < PLAYERNAME_SIZE && PLR2_OPT == selected_opt) {
			wattron(game_settings_menu_scr, A_BLINK);
			mvwaddch(game_settings_menu_scr, game_settings_menu_scr_h/2 - NO_OF_OPTS/2 + PLR2_OPT, game_settings_menu_scr_w/2 - (OPTS_SIZE-1)/2 + get_option_offset(options[PLR2_OPT]) + 1 + plr2_name_len, '_');
			wattroff(game_settings_menu_scr, A_BLINK);
		}
		wattroff(game_settings_menu_scr, A_STANDOUT);

		// display timer value
		if (TIMER_OPT == selected_opt)
			wattron(game_settings_menu_scr, A_STANDOUT);
		if (selected_timer == 0)
			mvwaddnstr(game_settings_menu_scr, game_settings_menu_scr_h/2 - NO_OF_OPTS/2 + TIMER_OPT, game_settings_menu_scr_w/2 - (OPTS_SIZE-1)/2 + get_option_offset(options[TIMER_OPT]) + 1, "<   off   >", 11);
		else
			mvwprintw(game_settings_menu_scr, game_settings_menu_scr_h/2 - NO_OF_OPTS/2 + TIMER_OPT, game_settings_menu_scr_w/2 - (OPTS_SIZE-1)/2 + get_option_offset(options[TIMER_OPT]) + 1, "< %2d mins >", timer_opts[selected_timer]);
		wattroff(game_settings_menu_scr, A_STANDOUT);

		// layout for START and CANCEL button
		if (START_OPT == selected_opt)
			wattron(game_settings_menu_scr, A_STANDOUT);
		mvwaddnstr(game_settings_menu_scr, game_settings_menu_scr_h/2 - NO_OF_OPTS/2 + (NO_OF_OPTS - 1), game_settings_menu_scr_w/2 - (OPTS_SIZE-1)/2, options[START_OPT], strlen(options[START_OPT]));
		if (CANCEL_OPT == selected_opt)
			wattron(game_settings_menu_scr, A_STANDOUT);
		else
			wattroff(game_settings_menu_scr, A_STANDOUT);
		mvwaddnstr(game_settings_menu_scr, game_settings_menu_scr_h/2 - NO_OF_OPTS/2 + (NO_OF_OPTS - 1), game_settings_menu_scr_w/2 - (OPTS_SIZE-1)/2 + strlen(options[START_OPT]), options[CANCEL_OPT], strlen(options[CANCEL_OPT]));
		wattroff(game_settings_menu_scr, A_STANDOUT);

		wrefresh(stdscr);
		wrefresh(game_settings_menu_scr);
		key = wgetch(game_settings_menu_scr);
	}

	delwin(game_settings_menu_scr);
	return CANCEL;
}


enum return_option_t init_ai_game_settings_menu (game_settings_t *game_settings) {
	player_name_t plr_name;
	int plr_name_len = 0;
	face_t play_as = WHITE;
	char difficulty_opts[][7] = { " Easy ", "Medium", " Hard " };
	int selected_difficulty = 0;
	int no_of_difficulties = sizeof(difficulty_opts)/sizeof(difficulty_opts[0]);
	int timer_opts[] = { 0, 3, 5, 10, 15, 30, 60 };	// in mins, 0 is off
	int selected_timer = 0;
	int no_of_timers = sizeof(timer_opts)/sizeof(timer_opts[0]);

	// storing values of window's (stdscr) size
	getmaxyx(stdscr, term_h, term_w);

	int key = -1;

	const int NO_OF_OPTS = 6;
	const int OPTS_SIZE = 10 + PLAYERNAME_SIZE;

	enum {PLR_OPT, PLAY_AS_OPT, DIFFICULTY_OPT, TIMER_OPT, START_OPT, CANCEL_OPT};

	// initialize options
	char options[NO_OF_OPTS][OPTS_SIZE+1];
	snprintf(options[PLR_OPT], OPTS_SIZE, "%-*s", OPTS_SIZE, "P1: ");
	snprintf(options[PLAY_AS_OPT], OPTS_SIZE, "%-*s", OPTS_SIZE, "Play as: ");
	snprintf(options[DIFFICULTY_OPT], OPTS_SIZE, "%-*s", OPTS_SIZE, "Level: ");
	snprintf(options[TIMER_OPT], OPTS_SIZE, "%-*s", OPTS_SIZE, "Timer: ");
	// separate layout for these two options
	snprintf(options[START_OPT], OPTS_SIZE, "%-*s", OPTS_SIZE/2, "Start");
	snprintf(options[CANCEL_OPT], OPTS_SIZE, "%-*s", OPTS_SIZE/2, "Cancel");
	int selected_opt = 0;

	// custom macro in src/utils/common.h to create new win with parameters associated with window's name
	initialize_with_box(game_settings_menu_scr);
	wrefresh(stdscr);
	wrefresh(game_settings_menu_scr);

	while (true) {
		box(game_settings_menu_scr, 0, 0);

		if (key == KEY_RESIZE) {
			getmaxyx(stdscr, term_h, term_w);
			translate_with_box(game_settings_menu_scr);
			wclear(stdscr);
		}

		// handle name input
		if (selected_opt == PLR_OPT) {
			if ((plr_name_len < PLAYERNAME_SIZE) && ((key >= 'a' && key <= 'z') || (key >= 'A' && key <= 'Z') || (key >= '0' && key <= '9') || key == '_')) {
				plr_name[plr_name_len++] = key;
			} else if (plr_name_len > 0 && key == KEY_BACKSPACE) {
				plr_name_len--;
				wclear(game_settings_menu_scr);
				box(game_settings_menu_scr, 0, 0);
			}
		}

		// handle play as input
		if (selected_opt == PLAY_AS_OPT) {
			if (key == KEY_LEFT || key == 'h' || key == KEY_RIGHT || key == 'l')
				play_as = (play_as == BLACK) ? WHITE: BLACK;
		}

		// handle difficulty input
		if (selected_opt == DIFFICULTY_OPT) {
			if (key == KEY_LEFT || key == 'h')
				selected_difficulty = (selected_difficulty - 1 + no_of_difficulties) % no_of_difficulties;
			else if (key == KEY_RIGHT || key == 'l')
				selected_difficulty = (selected_difficulty + 1) % no_of_difficulties;
		}

		// handle timer input
		if (selected_opt == TIMER_OPT) {
			if (key == KEY_LEFT || key == 'h')
				selected_timer = (selected_timer - 1 + no_of_timers) % no_of_timers;
			else if (key == KEY_RIGHT || key == 'l')
				selected_timer = (selected_timer + 1) % no_of_timers;
		}

		// handle menu navigation (don't allow cycling as layout isn't continuous)
		if (key == KEY_UP || (selected_opt != PLR_OPT && key == 'k'))
			selected_opt = max(selected_opt-1, 0);
		else if (key == KEY_DOWN || (selected_opt != PLR_OPT && key == 'j'))
			selected_opt = min(selected_opt+1, NO_OF_OPTS-1);
		else if (key == '\n' || key == '\r' || key == KEY_ENTER) {
			if (selected_opt == START_OPT) {
				plr_name[plr_name_len] = '\0';
				init_player(game_settings->players, plr_name, HUMAN);
				switch (selected_difficulty) {
					case 0:
						init_player(game_settings->players+1, "AI LVL 1", AI_LVL1);
						break;
					case 1:
						init_player(game_settings->players+1, "AI LVL 2", AI_LVL2);
						break;
					case 2:
						init_player(game_settings->players+1, "AI LVL 3", AI_LVL3);
						break;
				}
				game_settings->game_mode = AI_MODE;
				game_settings->time_limit = timer_opts[selected_timer];

				delwin(game_settings_menu_scr);
				return OKAY;
			} else if (selected_opt == CANCEL_OPT) {
				delwin(game_settings_menu_scr);
				return CANCEL;
			} else {
				selected_opt = min(selected_opt+1, NO_OF_OPTS-1);
			}
		}


		// layout for options before START button
		for (int i=0; i<START_OPT; i++) {
			if (i == selected_opt)
				wattron(game_settings_menu_scr, A_STANDOUT);
			mvwaddnstr(game_settings_menu_scr, game_settings_menu_scr_h/2 - NO_OF_OPTS/2 + i, game_settings_menu_scr_w/2 - (OPTS_SIZE-1)/2, options[i], OPTS_SIZE);

			wattroff(game_settings_menu_scr, A_STANDOUT);
		}

		// display player name
		if (PLR_OPT == selected_opt)
			wattron(game_settings_menu_scr, A_STANDOUT);
		mvwaddnstr(game_settings_menu_scr, game_settings_menu_scr_h/2 - NO_OF_OPTS/2 + PLR_OPT, game_settings_menu_scr_w/2 - (OPTS_SIZE-1)/2 + get_option_offset(options[PLR_OPT]) + 1, plr_name, plr_name_len);
		if (plr_name_len < PLAYERNAME_SIZE && PLR_OPT == selected_opt) {
			wattron(game_settings_menu_scr, A_BLINK);
			mvwaddch(game_settings_menu_scr, game_settings_menu_scr_h/2 - NO_OF_OPTS/2 + PLR_OPT, game_settings_menu_scr_w/2 - (OPTS_SIZE-1)/2 + get_option_offset(options[PLR_OPT]) + 1 + plr_name_len, '_');
			wattroff(game_settings_menu_scr, A_BLINK);
		}
		wattroff(game_settings_menu_scr, A_STANDOUT);

		// display play as value
		if (PLAY_AS_OPT == selected_opt)
			wattron(game_settings_menu_scr, A_STANDOUT);
		mvwprintw(game_settings_menu_scr, game_settings_menu_scr_h/2 - NO_OF_OPTS/2 + PLAY_AS_OPT, game_settings_menu_scr_w/2 - (OPTS_SIZE-1)/2 + get_option_offset(options[PLAY_AS_OPT]) + 1, "< %s >", (play_as == BLACK ? "Black": "White"));
		wattroff(game_settings_menu_scr, A_STANDOUT);

		// display difficulty value
		if (DIFFICULTY_OPT == selected_opt)
			wattron(game_settings_menu_scr, A_STANDOUT);
		mvwprintw(game_settings_menu_scr, game_settings_menu_scr_h/2 - NO_OF_OPTS/2 + DIFFICULTY_OPT, game_settings_menu_scr_w/2 - (OPTS_SIZE-1)/2 + get_option_offset(options[DIFFICULTY_OPT]) + 1, "< %-s >", difficulty_opts[selected_difficulty]);
		wattroff(game_settings_menu_scr, A_STANDOUT);

		// display timer value
		if (TIMER_OPT == selected_opt)
			wattron(game_settings_menu_scr, A_STANDOUT);
		if (selected_timer == 0)
			mvwaddnstr(game_settings_menu_scr, game_settings_menu_scr_h/2 - NO_OF_OPTS/2 + TIMER_OPT, game_settings_menu_scr_w/2 - (OPTS_SIZE-1)/2 + get_option_offset(options[TIMER_OPT]) + 1, "<   off   >", 11);
		else
			mvwprintw(game_settings_menu_scr, game_settings_menu_scr_h/2 - NO_OF_OPTS/2 + TIMER_OPT, game_settings_menu_scr_w/2 - (OPTS_SIZE-1)/2 + get_option_offset(options[TIMER_OPT]) + 1, "< %2d mins >", timer_opts[selected_timer]);
		wattroff(game_settings_menu_scr, A_STANDOUT);

		// layout for START and CANCEL button
		if (START_OPT == selected_opt)
			wattron(game_settings_menu_scr, A_STANDOUT);
		mvwaddnstr(game_settings_menu_scr, game_settings_menu_scr_h/2 - NO_OF_OPTS/2 + (NO_OF_OPTS - 1), game_settings_menu_scr_w/2 - (OPTS_SIZE-1)/2, options[START_OPT], strlen(options[START_OPT]));
		if (CANCEL_OPT == selected_opt)
			wattron(game_settings_menu_scr, A_STANDOUT);
		else
			wattroff(game_settings_menu_scr, A_STANDOUT);
		mvwaddnstr(game_settings_menu_scr, game_settings_menu_scr_h/2 - NO_OF_OPTS/2 + (NO_OF_OPTS - 1), game_settings_menu_scr_w/2 - (OPTS_SIZE-1)/2 + strlen(options[START_OPT]), options[CANCEL_OPT], strlen(options[CANCEL_OPT]));
		wattroff(game_settings_menu_scr, A_STANDOUT);

		wrefresh(stdscr);
		wrefresh(game_settings_menu_scr);
		key = wgetch(game_settings_menu_scr);
	}

	delwin(game_settings_menu_scr);
	return CANCEL;
}


/*
 * Returns the length of prefix ending with ':' or length of string if no such prefix is found.
 * This function is used to find the offset of options like 'P1:' to display value.
 */
static int get_option_offset (const char *const option_str) {
	int offset = 0;
	while (option_str[offset] != '\0' && option_str[offset] != ':')
		offset++;
	return (option_str[offset] == '\0' ? offset : offset + 1);
}
