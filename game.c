#include <stdlib.h>
#include <string.h>

#include "game.h"
#include "utilities.h"
#include "board.h"
#include "chess_engine.h"


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

static	void	draw_tile		(int y, int x, bool is_cur, bool is_sel, bool is_avail);
static	void	game_over		(const board_t *board);
static	void	del_game_wins	(void);

void init_game() {
	board_t board;
	init_board(&board);
	tile_t	**moves = NULL;
	int key = -1;
	onboard = true;
	short cur_tile[2] = {0, 0};
	short sel_tile[2] = {INVALID_ROW, INVALID_COL};

	getmaxyx(stdscr, term_h, term_w);
	printf("term_h: %d, term_w: %d", term_h, term_w);
	printf("window_w: %d", window_w);

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

	while (key != 'q') {
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
			if (key == '\n' || key == '\r' || key == KEY_ENTER) {
				if (sel_tile[0] != INVALID_ROW && sel_tile[1] != INVALID_ROW) {
					if (move_piece(&board, cur_tile, sel_tile) && is_game_finished(&board)) {
						game_over(&board);
						break;
					}

					clear_dest(&board);
					sel_tile[0] = INVALID_ROW;
					sel_tile[1] = INVALID_COL;
					if (moves != NULL)
						free(moves);
				} else if (board.tiles[cur_tile[0]][cur_tile[1]].piece != NULL && (board.tiles[cur_tile[0]][cur_tile[1]].piece->face & COLOR_BIT) == board.chance){
					sel_tile[0] = cur_tile[0];
					sel_tile[1] = cur_tile[1];
					moves = find_moves(&board, &board.tiles[sel_tile[0]][sel_tile[1]]);
					set_dest(&board, moves);
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

		for (short i=0; i<=8; i++) {
			for (short j=0; j<=8; j++) {
				if (i == 8 && j == 0) continue;
				int tile_row = 7-i, tile_col = j-1;

				wattroff(board_scr, A_BOLD);
				if ((tile_row + tile_col)%2 == 0)
					wattron(board_scr, A_STANDOUT);
				if (i < 8 && j > 0)
					draw_tile(i, j, (tile_row == cur_tile[0] && tile_col == cur_tile[1]), (tile_row == sel_tile[0] && tile_col == sel_tile[1]), board.tiles[tile_row][tile_col].can_be_dest);
				wattroff(board_scr, A_STANDOUT);

				if (i == 8) {
					char c[2] = {0};
					c[0] = 'A'+(j-1);
					mvwaddstr(board_scr, i*(tile_size_h + TILE_PAD_h) + tile_size_h/2, j*(tile_size_w + TILE_PAD_w) + tile_size_w/2, c);
				} else if (j == 0) {
					mvwaddstr(board_scr, i*(tile_size_h + TILE_PAD_h) + tile_size_h/2, j*(tile_size_w + TILE_PAD_w) + tile_size_w/2, itoa(8-i));
				} else {
					wattron(board_scr, A_BOLD);
					if ((tile_row + tile_col)%2 == 0) wattron(board_scr, A_STANDOUT);

					wchar_t piece[2] = {0};
					piece[0] = get_piece_face(board.tiles[tile_row][tile_col].piece);
					mvwaddwstr(board_scr, i*(tile_size_h + TILE_PAD_h) + tile_size_h/2, j*(tile_size_w + TILE_PAD_w) + tile_size_w/2, piece);

					wattroff(board_scr, A_STANDOUT);
				}
			}
		}

		wrefresh(stdscr);
		wrefresh(game_scr);
		wrefresh(menu_scr);
		wrefresh(plr1_scr);
		wrefresh(plr2_scr);
		wrefresh(board_scr);
		wrefresh(hud_scr);
		key = wgetch(game_scr);
	}

	del_game_wins();
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


static void game_over (const board_t *board) {
	if (board->result == PENDING)
		return;

	del_game_wins();

	initialize_with_box(game_over_scr);

	const int NO_OF_OPTS = 4;
	const int OPTS_SIZE = 12;

	// initialize options
	char options[NO_OF_OPTS][OPTS_SIZE+1];
	snprintf(options[0], OPTS_SIZE, "%-11s", "Undo");
	snprintf(options[1], OPTS_SIZE, "%-11s", "Save");
	snprintf(options[2], OPTS_SIZE, "%-11s", "Restart");
	snprintf(options[3], OPTS_SIZE, "%-11s", "Main Menu");
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
		exit(0);

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

		if (key == KEY_UP || key == 'k')
			selected_opt = (selected_opt-1 + NO_OF_OPTS) % NO_OF_OPTS;
		else if (key == KEY_DOWN || key == 'j')
			selected_opt = (selected_opt+1) % NO_OF_OPTS;
		else if (key == '\n' || key == '\r' || key == KEY_ENTER) {
			if (selected_opt == 2) {
				delwin(game_over_scr);
				init_game();
				return;
			}
			else if (selected_opt == 3) {
				delwin(game_over_scr);
				return ;
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
}


static void del_game_wins (void) {
	delwin(game_scr);
	delwin(menu_scr);
	delwin(plr1_scr);
	delwin(plr2_scr);
	delwin(board_scr);
	delwin(hud_scr);
}
