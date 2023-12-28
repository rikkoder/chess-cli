#include <ncurses.h>
#include <wchar.h>
#include <string.h>
#include <stdlib.h>
#include <dirent.h>
#include <limits.h>
#include <errno.h>

#include "main_menu.h"
#include "game.h"
#include "utilities.h"


static	enum	load_return_code	{LOAD_FILE_SELECTED, LOAD_FILE_NOT_SELECTED};

static	WINDOW	*main_menu_scr;
static	WINDOW	*load_menu_scr;
static	int		term_h;
static	int		term_w;

static	enum load_return_code	load_menu							(timestamp_t load_timestamp);
static	timestamp_t*			collect_save_files					(int *saves_count);
static	int						desc_timestamp_cmp					(char *t1, char *t2);
static	void					show_load_menu_title				(void);
static	void					show_save_files						(timestamp_t *saves, unsigned int saves_count, unsigned int cur_save);
static	void					convert_timestamp_to_display_format	(char *timestamp_display, timestamp_t timestamp);
static	void					show_load_menu_bar					(void);


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

	// custom macro in game.h to create new win with parameters associated with window's name
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
					 return_code = init_game();
				} while (return_code == RESTART);

				// redrawing after end of game
				// size may have changed while playing game
				getmaxyx(stdscr, term_h, term_w);
				translate_with_box(main_menu_scr);
				wclear(stdscr);
			} else if (selected_opt == LOAD_OPT) {
				timestamp_t load_timestamp;
				memset(load_timestamp, 0, sizeof(load_timestamp));
				enum load_return_code return_code = load_menu(load_timestamp);
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


static enum load_return_code load_menu (timestamp_t load_timestamp) {
	initialize_with_box(load_menu_scr);

	int saves_count = 0;
	timestamp_t *saves = collect_save_files(&saves_count);

	unsigned int cur_save = 0;
	enum load_return_code return_code = LOAD_FILE_NOT_SELECTED;

	int key = -1;
	while (true) {
		if (key == KEY_RESIZE) {
			getmaxyx(stdscr, term_h, term_w);
			translate_with_box(load_menu_scr);
			wclear(stdscr);
		}

		// handle menu keys
		if (key == 'n') {
			if ((cur_save/LOAD_PAGE_SIZE + 1) * LOAD_PAGE_SIZE < saves_count)
				cur_save = (cur_save/LOAD_PAGE_SIZE + 1) * LOAD_PAGE_SIZE;
		} else if (key == 'p') {
			if (cur_save >= LOAD_PAGE_SIZE)
				cur_save = (cur_save/LOAD_PAGE_SIZE - 1) * LOAD_PAGE_SIZE;
		} else if (key == 'q') {
			return_code = LOAD_FILE_NOT_SELECTED;
			break;
		}

		// handle file selection
		else if (key == KEY_UP || key == 'k') {
			if (cur_save > 0)
				cur_save--;
		} else if (key == KEY_DOWN || key == 'j') {
			if (cur_save + 1 < saves_count)
				cur_save++;
		} else if (key == KEY_LEFT || key == 'h') {
			if (cur_save >= LOAD_LIST_SIZE)
				cur_save -= LOAD_LIST_SIZE;
		} else if (key == KEY_RIGHT || key == 'l') {
			if (cur_save + LOAD_LIST_SIZE < saves_count)
				cur_save += LOAD_LIST_SIZE;
		} else if (key == '\n' || key == '\r' || key == KEY_ENTER) {
			strncpy(load_timestamp, saves[cur_save], TIMESTAMP_SIZE);
			return_code = LOAD_FILE_SELECTED;
			break;
		}

		wclear(load_menu_scr);
		box(load_menu_scr, 0, 0);

		show_load_menu_title();
		show_save_files(saves, saves_count, cur_save);
		show_load_menu_bar();

		wrefresh(load_menu_scr);

		key = wgetch(load_menu_scr);
	}

	wclear(load_menu_scr);
	wrefresh(load_menu_scr);
	delwin(load_menu_scr);

	free(saves);

	return return_code;
}


static timestamp_t* collect_save_files (int *saves_count) {
	unsigned int saves_capacity = 50;
	timestamp_t *saves = (timestamp_t *) malloc(saves_capacity * sizeof(timestamp_t));
	memset(saves, 0, saves_capacity * sizeof(timestamp_t));

	int _saves_count = 0;
	DIR *dir;
	dir = opendir(save_directory);
	if (dir == NULL) {
		fprintf(stderr, "%s: can't open save location..", strerror(errno));
		exit(EXIT_FAILURE);
	}
	struct dirent *subdir;
	while ((subdir = readdir(dir)) != NULL) {
		if (strlen(subdir->d_name) < strlen(FILE_EXT))
			continue;
		if (strncmp(FILE_EXT, subdir->d_name + strlen(subdir->d_name) - strlen(FILE_EXT), strlen(FILE_EXT)) == 0) {
			if (_saves_count < saves_capacity) {
				strncpy(saves[_saves_count++], subdir->d_name, TIMESTAMP_SIZE);
			} else {
				if (saves_capacity >= UINT_MAX/2) {
					fprintf(stderr, "can't load.. too much save files!");
					exit(EXIT_FAILURE);
				}
				saves_capacity <<= 1;
				saves = realloc(saves, saves_capacity * sizeof(timestamp_t));
				if (saves == NULL) {
					fprintf(stderr, "can't load.. too much save files!");
					exit(EXIT_FAILURE);
				}
				strncpy(saves[_saves_count++], subdir->d_name, TIMESTAMP_SIZE);
			}
		}
	}
	closedir(dir);

	qsort(saves, _saves_count, sizeof(saves[0]), desc_timestamp_cmp);

	*saves_count = _saves_count;
	return saves;
}


static int desc_timestamp_cmp (char *t1, char *t2) {
	return strncmp(t2, t1, TIMESTAMP_SIZE);
}


static void show_load_menu_title (void) {
	wattron(load_menu_scr, A_BOLD);
	wattron(load_menu_scr, A_UNDERLINE);
	mvwaddnstr(load_menu_scr, 1, load_menu_scr_w/2 - 2, "LOAD", 4);
	wattroff(load_menu_scr, A_UNDERLINE);
	wattroff(load_menu_scr, A_BOLD);
}


static void show_save_files (timestamp_t *saves, unsigned int saves_count, unsigned int cur_save) {
	unsigned int cur_index = (cur_save / LOAD_PAGE_SIZE) * LOAD_PAGE_SIZE;
	for (int i = 0; i < NO_OF_LOAD_LISTS && cur_index < saves_count; i++) {
		for (int j = 0; j < LOAD_LIST_SIZE && cur_index < saves_count; j++) {
			wattroff(load_menu_scr, A_STANDOUT);
			if (cur_index == cur_save)
				wattron(load_menu_scr, A_STANDOUT);
			char timestamp_display[TIMESTAMP_DISPLY_SIZE+1];
			convert_timestamp_to_display_format(timestamp_display, saves[cur_index]);
			mvwaddnstr(load_menu_scr, 2+j, LOAD_EXTRA_PAD + LOAD_LIST_PAD+ i*(LOAD_LIST_PAD + TIMESTAMP_DISPLY_SIZE), timestamp_display, TIMESTAMP_DISPLY_SIZE);
			cur_index++;
		}
	}

	wattroff(load_menu_scr, A_STANDOUT);
	const unsigned int PAGE_NO_SIZE = 10;
	char page_no[PAGE_NO_SIZE + 1];
	snprintf(page_no, PAGE_NO_SIZE+1, "%d/%d", cur_save/LOAD_PAGE_SIZE + 1, saves_count/LOAD_PAGE_SIZE + 1);
	char page_no_str[PAGE_NO_SIZE+1];
	snprintf(page_no_str, PAGE_NO_SIZE+1, "%*s", PAGE_NO_SIZE, page_no);
	mvwaddnstr(load_menu_scr, load_menu_scr_h - 3, load_menu_scr_w - LOAD_EXTRA_PAD - LOAD_LIST_PAD - PAGE_NO_SIZE - 1, page_no_str, PAGE_NO_SIZE);
}


static void convert_timestamp_to_display_format (char *timestamp_display, timestamp_t timestamp) {
	// timestamp format -			YYYYMMDDHHMMSS
	// timestamp display format -	HH:MM DD MON YY
	char months[][4] = { "JAN", "FEB", "MAR", "APR", "MAY", "JUN", "JUL", "AUG", "SEP", "OCT", "NOV", "DEC" };
	int month_index = (timestamp[4] - '0') * 10 + (timestamp[5] - '0') - 1;
	snprintf(timestamp_display, TIMESTAMP_DISPLY_SIZE+1, "%c%c:%c%c %c%c %s %c%c\0", timestamp[8], timestamp[9], timestamp[10], timestamp[11], timestamp[6], timestamp[7], months[month_index], timestamp[2], timestamp[3]);
}


static void show_load_menu_bar (void) {
	const int NO_OF_OPTS = 3;
	const int OPTS_SIZE = 12;
	const int V_OFFSET = load_menu_scr_h - 2, H_OFFSET = 1;

	// initialize options
	char options[NO_OF_OPTS][OPTS_SIZE+1];
	char prefixes[] = {'n', 'p', 'q'};
	snprintf(options[0], OPTS_SIZE, ": %s", "next page");
	snprintf(options[1], OPTS_SIZE, ": %s", "prev page");
	snprintf(options[2], OPTS_SIZE, ": %s", "quit");

	for (int i=0; i<NO_OF_OPTS; i++) {
		wattron(load_menu_scr, A_BOLD);
		wattron(load_menu_scr, A_STANDOUT);
		int prefix_pos = (i*(OPTS_SIZE+1)) + H_OFFSET;
		mvwaddch(load_menu_scr, V_OFFSET, prefix_pos, prefixes[i]);
		wattroff(load_menu_scr, A_BOLD);
		wattroff(load_menu_scr, A_STANDOUT);
		mvwaddnstr(load_menu_scr, V_OFFSET, prefix_pos + 1, options[i], OPTS_SIZE);
	}
}

