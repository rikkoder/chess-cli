#include "game.h"

static WINDOW *game, *menu_bar, *plr1, *plr2, *board, *hud;
static int term_h, term_w;
// static const int PAD = 2;
// static int term_w, term_h, game_w, game_h, 

// static void init_win(WINDOW *win);
static void init_board(WINDOW *board, WINDOW *game);

void init_game() {
// 	int term_w, term_h, game_w, game_h;
	int key = -1;

// 	const int PAD = 2;

	getmaxyx(stdscr, term_h, term_w);

	// custom macro in game.h to create new win with parameters associated with window's name
	initialize_with_macro(game);
	initialize_with_macro(menu_bar);
	initialize_with_macro(plr1);
	initialize_with_macro(plr2);
	initialize_with_macro(board);
	initialize_with_macro(hud);

// 	game_w = term_w - 2*PAD;
// 	game_h = term_h - 2*PAD;

// 	WINDOW *game = newwin(game_h, game_w, PAD, PAD);
// 	game = newwin(game_h, game_w, game_y, game_x);
// 	game = makewin(game);
// 	keypad(game, TRUE);
// 	box(game, 0, 0);

// 	plr1 = newwin(plr_h, plr_w, plr1_y, plr1_x);
// 	plr1 = makewin(plr1);
// 	keypad(plr1, TRUE);
// 	box(plr1, 0, 0);

// 	board = newwin(board_h, board_w, board_y, board_)
// 	board = makewin(board);

// 	WINDOW *menu_bar = newwin(menu_bar_h, menu_bar_w, game_h - menu_bar_h + PAD, PAD);
	menu_bar = newwin(menu_bar_h, menu_bar_w, menu_bar_y, menu_bar_x);
	keypad(menu_bar, TRUE);
// 	box(menu_bar, 0, 0);

	wrefresh(stdscr);
	wrefresh(game);
	wrefresh(menu_bar);

	while (key != 'q') {
		if (key == KEY_RESIZE) {
#ifdef PDCURSES
			resize_term(0, 0);
#endif
			getmaxyx(stdscr, term_h, term_w);
// 			game_w = term_w - 2*PAD;
// 			game_h = term_h - 2*PAD;

// 			wresize(game, game_h, game_w);
// 			mvwin(game, game_y, game_x);
// 			wresize(menu_bar, menu_bar_h, menu_bar_w);
// 			mvwin(menu_bar, menu_bar_y, menu_bar_x);
// 
// 			wclear(stdscr);
// 			wclear(game);
// 			wclear(menu_bar);

			// custom macro in game.h to resize, move and clear window
			translate_with_macro(game);
			translate_with_macro(menu_bar);
			translate_with_macro(plr1);
			translate_with_macro(plr2);
			translate_with_macro(board);
			translate_with_macro(hud);
			wclear(stdscr);

// 			box(game, 0, 0);
// 			box(menu_bar, 0, 0);
		}

		mvwaddstr(game, 1, 1, "game");
		mvwaddstr(menu_bar, 1, 1, "menu_bar");
		mvwaddstr(plr1, 1, 1, "plr1");
		mvwaddstr(plr2, 1, 1, "plr2");
		mvwaddstr(board, 1, 1, "board");
		mvwaddstr(hud, 1, 1, "hud");

		wrefresh(stdscr);
		wrefresh(game);
		wrefresh(menu_bar);
		wrefresh(plr1);
		wrefresh(plr2);
		wrefresh(board);
		wrefresh(hud);
		key = wgetch(game);
	}

	delwin(menu_bar);
	delwin(game);
}


static void init_board(WINDOW *board, WINDOW *game) {
	printf("i am static\n");
}

// static void init_win(WINDOW *win) {
// 	default_initialize(win);
// 	keypad(win, TRUE);
// }
