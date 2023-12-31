#ifndef GAME_H
#define GAME_H

#include "main.h"
#include "history.h"

// to make stdscr compatible with init and trans macros
#define stdscr_h term_h
#define stdscr_w term_w
#define stdscr_y 0
#define stdscr_x 0

#ifndef true
#define true 1
#endif
#ifndef false
#define false 0
#endif


// this gaurd was included so that these macros aren't defined for main_menu.c as not required by main_menu, but if main_menu.h is included in this file in future it would break program as following macros won't be defined. thus commenting main_menu_h gaurd below.
//#ifndef MAIN_MENU_H

#define PAD_h 0
#define PAD_w htow(PAD_h)
#define INNER_PAD_h 1
#define INNER_PAD_w htow(INNER_PAD_h)
#define TILE_PAD_h 0
#define TILE_PAD_w htow(TILE_PAD_h)
#define game_scr_h (window_h - 2 * PAD_h)
#define game_scr_w (window_w - 2 * PAD_w)
#define game_scr_y ((term_h - game_scr_h)/2)	// center of screen
#define game_scr_x ((term_w - game_scr_w)/2)	// center of screen
#define menu_scr_h 3
#define menu_scr_w game_scr_w
#define menu_scr_y (game_scr_y + game_scr_h - menu_scr_h)
#define menu_scr_x (game_scr_x)
#define tile_size_h 3 //((game_scr_h - 4 * INNER_PAD_h - menu_scr_h) / 11) // 8 (board_scr) + 2 (players) + 1 (col label)
#define tile_size_w htow(tile_size_h)
#define plr_h tile_size_h
#define plr_w (game_scr_w - 2 * INNER_PAD_w)
#define plr2_scr_h plr_h	// for makewin compatibility
#define plr2_scr_w plr_w	// for makewin compatibility
#define plr2_scr_y (game_scr_y + INNER_PAD_h)
#define plr2_scr_x (game_scr_x + INNER_PAD_w)
#define board_scr_h (9 * tile_size_h + 9 * TILE_PAD_h)
#define board_scr_w (9 * tile_size_w + 9 * TILE_PAD_w)
#define board_scr_y (plr2_scr_y + plr_h + INNER_PAD_h)
#define board_scr_x (game_scr_x + INNER_PAD_w)
#define plr1_scr_h plr_h	// for makewin compatibility
#define plr1_scr_w plr_w	// for makewin compatibility
#define plr1_scr_y (board_scr_y + board_scr_h + INNER_PAD_h)
#define plr1_scr_x plr2_scr_x
#define hud_scr_h board_scr_h
#define hud_scr_w (game_scr_w - board_scr_w - 3 * INNER_PAD_w)
#define hud_scr_y board_scr_y
#define hud_scr_x (board_scr_x + board_scr_w + INNER_PAD_w)
#define game_over_scr_h 10
#define game_over_scr_w htow(game_over_scr_h)
#define game_over_scr_y ((term_h - game_over_scr_h) / 2)	// center of screen
#define game_over_scr_x ((term_w - game_over_scr_w) / 2)	// center of screen


// gaurd close for main_menu.h
//#endif


enum game_return_code	{QUIT, RESTART, CONTINUE, INVALID_LOAD};

enum game_return_code	init_game	(const timestamp_t load_timestamp);


#endif
