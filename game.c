#include <stdlib.h>

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
static	int				term_h;
static	int				term_w;
static	bool			onboard;
static	chance_t		chance = WHITE;
static	const tile_t	**moves = NULL;

// static	void	draw_moves	();
static	void	draw_tile	(int y, int x, bool is_cur, bool is_sel, bool is_avail);
static 	void 	set_dest	(board_t *board);
static 	void 	clear_dest	(board_t *board);

void init_game() {
	board_t board;
	init_board(&board);
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
// #ifdef PDCURSES
// 			resize_term(0, 0);
// #endif
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
					if (move_piece(&board, cur_tile, sel_tile))
						chance = (chance == WHITE ? BLACK : WHITE);
					clear_dest(&board);
					sel_tile[0] = INVALID_ROW;
					sel_tile[1] = INVALID_COL;
					if (moves != NULL)
						free(moves);
				} else if (board.tiles[cur_tile[0]][cur_tile[1]].piece != NULL && (board.tiles[cur_tile[0]][cur_tile[1]].piece->face & COLOR_BIT) == chance){
					sel_tile[0] = cur_tile[0];
					sel_tile[1] = cur_tile[1];
					moves = find_moves(&board, &board.tiles[sel_tile[0]][sel_tile[1]]);
					set_dest(&board);
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

		mvwaddstr(game_scr, 1, 1, "game_scr");
		mvwaddstr(menu_scr, 1, 1, "menu_scr");
		mvwaddstr(plr1_scr, 1, 1, "plr1_scr");
		mvwaddstr(plr2_scr, 1, 1, "plr2_scr");

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

// 		draw_moves();

		mvwaddstr(hud_scr, 1, 1, "hud_scr");

		wrefresh(stdscr);
		wrefresh(game_scr);
		wrefresh(menu_scr);
		wrefresh(plr1_scr);
		wrefresh(plr2_scr);
		wrefresh(board_scr);
		wrefresh(hud_scr);
		key = wgetch(game_scr);
	}

	delwin(game_scr);
	delwin(menu_scr);
	delwin(plr1_scr);
	delwin(plr2_scr);
	delwin(board_scr);
	delwin(hud_scr);
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
// 		mvwaddch(board_scr, y, x, ' ');
// 		mvwaddch(board_scr, y, x+w-1, ' ');
// 		mvwaddch(board_scr, y+h-1, x, ' ');
// 		mvwaddch(board_scr, y+h-1, x+w-1, ' ');
	}
	if (is_cur) {
		mvwaddch(board_scr, y, x, ACS_ULCORNER);
		mvwaddch(board_scr, y, x+w-1, ACS_URCORNER);
		mvwaddch(board_scr, y+h-1, x, ACS_LLCORNER);
		mvwaddch(board_scr, y+h-1, x+w-1, ACS_LRCORNER);
	}
	wattroff(board_scr, A_BOLD);
}

/* 
static void draw_moves() {
	if (moves == NULL)
		return;
	for (int i=0; i<MAX_MOVES; i++) {
		if (moves[i] == NULL)
			return;
		int draw_row = 7-moves[i]->row, draw_col = moves[i]->col+1;
		draw_tile(draw_row, draw_col, false, false, true);
	}
}
 */

static void set_dest(board_t *board) {
	if (moves == NULL)
		return;
	for (int i=0; i<MAX_MOVES; i++) {
		// removing constness of pointer
		tile_t *tile = moves[i];
		if (tile == NULL)
			return;

		tile->can_be_dest = true;
	}
}


static void clear_dest(board_t *board) {
	for (int i=0; i<8; i++) {
		for (int j=0; j<8; j++) {
			board->tiles[i][j].can_be_dest = false;
		}
	}
}
