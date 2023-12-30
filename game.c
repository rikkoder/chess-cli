#include <stdlib.h>
#include <string.h>

#include "game.h"
#include "utilities.h"
#include "board.h"
#include "chess_engine.h"
#include "history.h"


static	WINDOW			*game_scr;
static	WINDOW			*menu_scr;
static	WINDOW			*plr1_scr;
static	WINDOW			*plr2_scr;
static	WINDOW			*board_scr;
static	WINDOW			*hud_scr;
static	WINDOW			*game_over_scr;
static	int				term_h;
static	int				term_w;
static	bool			onboard;

static	void					draw_board		(const board_t *board, const short sel_tile[2], const short cur_tile[2]);
static	void					draw_tile		(int y, int x, bool is_cur, bool is_sel, bool is_avail);
static	enum game_return_code	game_over		(board_t *board, history_t *history);
static	void					del_game_wins	(void);
static	void					show_history	(history_t *history);
static	void					show_captured	(const board_t *board);
static	void					show_menu		(void);
static	void					undo_game		(board_t *board, history_t *history);


enum game_return_code init_game(const timestamp_t load_timestamp) {
	enum game_return_code return_code = QUIT;

	// load this part from saved file to load a game
	board_t *board = (board_t *) calloc(1, sizeof(board_t));
	history_t *history = NULL;
	if (load_timestamp == NULL) {
		init_board(board);
		history = create_history();
	} else {
		history = load_hstk(load_timestamp);
		copy_board(board, peek_board(history, 0));
	}

	tile_t	**moves = NULL;
	int key = -1;
	onboard = true;
	short cur_tile[2] = {0, 0};
	short sel_tile[2] = {INVALID_ROW, INVALID_COL};

	getmaxyx(stdscr, term_h, term_w);

	// custom macro in game.h to create new win with parameters associated with window's name
	initialize_with_box(game_scr);
	initialize_with_box(menu_scr);
	initialize_with_box(plr1_scr);
	initialize_with_box(plr2_scr);
	initialize_without_box(board_scr);
	initialize_with_box(hud_scr);

	wrefresh(stdscr);
	wrefresh(game_scr);
	wrefresh(menu_scr);

	show_history(history);
	show_captured(board);

	while (true) {
		if (key == KEY_RESIZE) {
			getmaxyx(stdscr, term_h, term_w);

			// custom macro in game_scr.h to resize, move and clear window
			translate_with_box(game_scr);
			translate_with_box(menu_scr);
			translate_with_box(plr1_scr);
			translate_with_box(plr2_scr);
			translate_without_box(board_scr);
			translate_with_box(hud_scr);
			wclear(stdscr);
			
		} else if (onboard) {
			// handle menu keys
			if (key == 'q') {
				break;
			} else if (key == 'u') {
				undo_game(board, history);
				sel_tile[0] = INVALID_ROW;
				sel_tile[1] = INVALID_COL;
			} else if (key == 's') {
				save_hstk(history);
			} else if (key == 'a') {
				SETTINGS_UNICODE_MODE = !SETTINGS_UNICODE_MODE;
			}

			// handle board events
			else if (key == '\n' || key == '\r' || key == KEY_ENTER) {
				if (sel_tile[0] != INVALID_ROW && sel_tile[1] != INVALID_ROW) {
					if (move_piece(board, cur_tile, sel_tile, history)) {
						if (board->result != PENDING) { // result is calculated in move_piece
							if ((return_code = game_over(board, history)) != CONTINUE)
								break;
						}
					}

					clear_dest(board);
					sel_tile[0] = INVALID_ROW;
					sel_tile[1] = INVALID_COL;
					if (moves != NULL)
						free(moves);
				} else if (board->tiles[cur_tile[0]][cur_tile[1]].piece != NULL && (board->tiles[cur_tile[0]][cur_tile[1]].piece->face & COLOR_BIT) == board->chance){
					sel_tile[0] = cur_tile[0];
					sel_tile[1] = cur_tile[1];
					moves = find_moves(board, &board->tiles[sel_tile[0]][sel_tile[1]], history);
					set_dest(board, moves);
				}
			} else if ( key == KEY_UP || key == 'k') {
				if (cur_tile[0] < 7) cur_tile[0]++;
			} else if (key == KEY_DOWN || key == 'j') {
				if (cur_tile[0] > 0) cur_tile[0]--;
			} else if (key == KEY_LEFT || key == 'h') {
				if (cur_tile[1] > 0) cur_tile[1]--;
			} else if (key == KEY_RIGHT || key == 'l') {
				if (cur_tile[1] < 7) cur_tile[1]++;
			}
		}

		draw_board(board, sel_tile, cur_tile);
		show_history(history);
		show_captured(board);
		show_menu();

		wrefresh(stdscr);
		wrefresh(game_scr);
		wrefresh(menu_scr);
		wrefresh(plr1_scr);
		wrefresh(plr2_scr);
		wrefresh(board_scr);
		wrefresh(hud_scr);

		key = wgetch(game_scr);
	}

	delete_board(board);
	delete_history(history);
	del_game_wins();

	return return_code;
}


static void draw_board (const board_t *board, const short sel_tile[2], const short cur_tile[2]) {
	for (short i=0; i<=8; i++) {
		for (short j=0; j<=8; j++) {
			if (i == 8 && j == 0) continue;
			int tile_row = 7-i, tile_col = j-1;

			wattroff(board_scr, A_BOLD);
			if ((tile_row + tile_col)%2 != 0)
				wattron(board_scr, A_STANDOUT);
			if (i < 8 && j > 0)
				draw_tile(i, j, (tile_row == cur_tile[0] && tile_col == cur_tile[1]), (tile_row == sel_tile[0] && tile_col == sel_tile[1]), board->tiles[tile_row][tile_col].can_be_dest);
			wattroff(board_scr, A_STANDOUT);

			if (i == 8) {
				char c[2] = {0};
				c[0] = 'A'+(j-1);
				mvwaddstr(board_scr, i*(tile_size_h + TILE_PAD_h) + tile_size_h/2, j*(tile_size_w + TILE_PAD_w) + tile_size_w/2, c);
			} else if (j == 0) {
				char a[20];
				memset(a, 0, 20);
				itoa(8-i, a);
				mvwaddstr(board_scr, i*(tile_size_h + TILE_PAD_h) + tile_size_h/2, j*(tile_size_w + TILE_PAD_w) + tile_size_w/2, a);
			} else {
				wattron(board_scr, A_BOLD);
				if ((tile_row + tile_col)%2 != 0) wattron(board_scr, A_STANDOUT);

				wchar_t piece[2] = {0};
				piece[0] = get_piece_face(board->tiles[tile_row][tile_col].piece);
				mvwaddwstr(board_scr, i*(tile_size_h + TILE_PAD_h) + tile_size_h/2, j*(tile_size_w + TILE_PAD_w) + tile_size_w/2, piece);

				wattroff(board_scr, A_STANDOUT);
			}
		}
	}
}


static void draw_tile(int y, int x, bool is_cur, bool is_sel, bool can_be_dest) {
	y *= (tile_size_h + TILE_PAD_h);
	x *= (tile_size_w + TILE_PAD_w);
	int w = tile_size_w, h = tile_size_h;
	for (int i=0; i<h; i++) {
		for (int j=0; j<w; j++) {
			mvwaddch(board_scr, y+i, x+j, ' ');
		}
	}

	if (!onboard) return;

	wattron(board_scr, A_BOLD);
	if (is_sel) {
		mvwhline(board_scr, y, x, 'x', w);
		mvwhline(board_scr, y+h-1, x, 'x', w);
		mvwvline(board_scr, y, x, 'x', h);
		mvwvline(board_scr, y, x+w-1, 'x', h);
	}
	if (can_be_dest) {
		mvwhline(board_scr, y, x, '\'', w);
		mvwhline(board_scr, y+h-1, x, '\'', w);
		mvwvline(board_scr, y, x, '\'', h);
		mvwvline(board_scr, y, x+w-1, '\'', h);
	}
	if (is_cur) {
		mvwaddch(board_scr, y, x, ACS_ULCORNER);
		mvwaddch(board_scr, y, x+w-1, ACS_URCORNER);
		mvwaddch(board_scr, y+h-1, x, ACS_LLCORNER);
		mvwaddch(board_scr, y+h-1, x+w-1, ACS_LRCORNER);
	}
	wattroff(board_scr, A_BOLD);
}


static enum game_return_code game_over (board_t *board, history_t * history) {
	if (board->result == PENDING)
		return CONTINUE;

// 	del_game_wins();

	initialize_with_box(game_over_scr);

	bool is_saved = false;

	const int NO_OF_OPTS = 4;
	const int OPTS_SIZE = 12;

	enum {UNDO_OPT, SAVE_OPT, RESTART_OPT, MAIN_MENU_OPT};

	// initialize options
	char options[NO_OF_OPTS][OPTS_SIZE+1];
	snprintf(options[UNDO_OPT], OPTS_SIZE, "%-11s", "Undo");
	snprintf(options[SAVE_OPT], OPTS_SIZE, "%-11s", "Save");
	snprintf(options[RESTART_OPT], OPTS_SIZE, "%-11s", "Restart");
	snprintf(options[MAIN_MENU_OPT], OPTS_SIZE, "%-11s", "Main Menu");
	int selected_opt = 0;

	const int RESULT_SIZE = 20;

	char result[RESULT_SIZE];
	if (board->result == WHITE_WON)
		sprintf(result, "%s", "White Won");
	else if (board->result == BLACK_WON)
		sprintf(result, "%s", "Black Won");
	else if (board->result == STALE_MATE)
		sprintf(result, "%s", "Stale Mate");
	// error
	else
		exit(EXIT_FAILURE);

	wattron(game_over_scr, A_UNDERLINE);
	mvwaddnstr(game_over_scr, 1, game_over_scr_w/2 - strlen(result)/2, result, RESULT_SIZE);
	wattroff(game_over_scr, A_UNDERLINE);

	int key = -1;
	while (true) {
		if (key == KEY_RESIZE) {
			getmaxyx(stdscr, term_h, term_w);
			translate_with_box(game_over_scr);
			wclear(stdscr);
		}

		if (key == KEY_UP || key == 'k') {
			selected_opt = (selected_opt-1 + NO_OF_OPTS) % NO_OF_OPTS;
			if (selected_opt == SAVE_OPT && is_saved)
				selected_opt = (selected_opt-1 + NO_OF_OPTS) % NO_OF_OPTS;
		} else if (key == KEY_DOWN || key == 'j') {
			selected_opt = (selected_opt+1) % NO_OF_OPTS;
			if (selected_opt == SAVE_OPT && is_saved)
				selected_opt = (selected_opt+1) % NO_OF_OPTS;
		} else if (key == '\n' || key == '\r' || key == KEY_ENTER) {
			if (selected_opt == UNDO_OPT) {
				undo_game(board, history);
				return CONTINUE;
			} else if (selected_opt == SAVE_OPT) {
				save_hstk(history);
				is_saved = true;
				snprintf(options[SAVE_OPT], OPTS_SIZE, "%-11s", "Saved");
			} else if (selected_opt == RESTART_OPT) {
				delwin(game_over_scr);
				return RESTART;
			} else if (selected_opt == MAIN_MENU_OPT) {
				wclear(game_over_scr);
				wrefresh(game_over_scr);
				delwin(game_over_scr);
				return QUIT;
			}
		}

		for (int i=0; i<NO_OF_OPTS; i++) {
			if (i == selected_opt)
				wattron(game_over_scr, A_STANDOUT);
			else
				wattroff(game_over_scr, A_STANDOUT);

			mvwaddnstr(game_over_scr, game_over_scr_h/2 - NO_OF_OPTS/2 + i, game_over_scr_w/2 - (OPTS_SIZE-1)/2, options[i], OPTS_SIZE);
		}

		wrefresh(stdscr);
		wrefresh(game_over_scr);
		key = wgetch(game_over_scr);
	}

	// failsafe
	return QUIT;
}


static void del_game_wins (void) {
	wclear(game_scr);
	wrefresh(game_scr);
	delwin(game_scr);
	wclear(menu_scr);
	wrefresh(menu_scr);
	delwin(menu_scr);
	wclear(plr1_scr);
	wrefresh(plr1_scr);
	delwin(plr1_scr);
	wclear(plr2_scr);
	wrefresh(plr2_scr);
	delwin(plr2_scr);
	wclear(board_scr);
	wrefresh(board_scr);
	delwin(board_scr);
	wclear(hud_scr);
	wrefresh(hud_scr);
	delwin(hud_scr);
}


static void show_history (history_t *history) {
	wclear(hud_scr);
	box(hud_scr, 0, 0);
	int sz = min(get_size(history), hud_scr_h - 2);
	float move_no = get_size(history) - sz + 0.5;
	int move_display_max_size = MAX_MOVE_NOTATION_SIZE + 5;
	for (int i=1; i<=sz; i++) {
		char s[move_display_max_size];
		snprintf(s, move_display_max_size, "%.1f  %s", move_no, peek_move(history, sz-i));
		mvwaddnstr(hud_scr, i, 1, s, move_display_max_size);
		move_no += 0.5;
	}
	wrefresh(hud_scr);
}


static void show_captured (const board_t *board) {
	wclear(plr1_scr);
	box(plr1_scr, 0, 0);

// 	bool is_unicode = is_unicode(board->kings[0]->piece->face);
	// use SETTINGS_UNICODE_MODE
	bool is_unicode = SETTINGS_UNICODE_MODE;
	const int V_OFFSET = 1, H_OFFSET = 1, WSTR_SIZE = 7;
	wchar_t wstr[WSTR_SIZE];
	for (int i=0; i<PIECE_TYPES; i++) {
		if ((1 << i) & KING) continue;	// king is never captured
		swprintf(wstr, WSTR_SIZE, L"%lcx%hd", PIECES[is_unicode][1][i], board->captured[1][i]);
		mvwaddnwstr(plr1_scr, V_OFFSET, (i*WSTR_SIZE) + H_OFFSET, wstr, WSTR_SIZE);
		swprintf(wstr, WSTR_SIZE, L"%lcx%hd", PIECES[is_unicode][0][i], board->captured[0][i]);
		mvwaddnwstr(plr2_scr, V_OFFSET, (i*WSTR_SIZE) + H_OFFSET, wstr, WSTR_SIZE);
	}

	wrefresh(plr1_scr);
	wrefresh(plr2_scr);
}


static void show_menu (void) {
	const int NO_OF_OPTS = 4;
	const int OPTS_SIZE = 12;
	const int V_OFFSET = 1, H_OFFSET = 1;

	// initialize options
	char options[NO_OF_OPTS][OPTS_SIZE+1];
	char prefixes[] = {'a', 'u', 's', 'q'};
	if (SETTINGS_UNICODE_MODE)
		snprintf(options[0], OPTS_SIZE, ": %s", "ascii");
	else
		snprintf(options[0], OPTS_SIZE, ": %s", "unicode");
	snprintf(options[1], OPTS_SIZE, ": %s", "undo");
	snprintf(options[2], OPTS_SIZE, ": %s", "save");
	snprintf(options[3], OPTS_SIZE, ": %s", "quit");

	for (int i=0; i<NO_OF_OPTS; i++) {
		wattron(menu_scr, A_BOLD);
		wattron(menu_scr, A_STANDOUT);
		int prefix_pos = (i*(OPTS_SIZE+1)) + H_OFFSET;
		mvwaddch(menu_scr, V_OFFSET, prefix_pos, prefixes[i]);
		wattroff(menu_scr, A_BOLD);
		wattroff(menu_scr, A_STANDOUT);
		mvwaddnstr(menu_scr, V_OFFSET, prefix_pos + 1, options[i], OPTS_SIZE);
	}

	wrefresh(menu_scr);
}


static void undo_game (board_t *board, history_t *history) {
	undo(history);
	const board_t *prev_board = peek_board(history, 0);
	if (prev_board == NULL)
		init_board(board);
	else
		copy_board(board, peek_board(history, 0));
	clear_dest(board);
}
