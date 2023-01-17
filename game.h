#ifndef GAME_H
#define GAME_H

#include "main.h"
#include <ncurses.h>

// without boxes
#define initialize_without_box(x)	{x = newwin(x##_h, x##_w, x##_y, x##_x); keypad(x, true);}
#define translate_without_box(x)		{wresize(x, x##_h, x##_w); mvwin(x, x##_y, x##_x); wclear(x);}

// with boxes
#define initialize_with_box(x)	{x = newwin(x##_h, x##_w, x##_y, x##_x); keypad(x, true); box(x, 0, 0);}
#define translate_with_box(x)		{wresize(x, x##_h, x##_w); mvwin(x, x##_y, x##_x); wclear(x); box(x, 0, 0);}

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


#ifndef MAIN_MENU_H

// for dynamic sizing
/*
#define PAD 2
#define game_scr_h (term_h - 2 * PAD)
#define game_scr_w (term_w - 2 * PAD)
#define game_scr_y 2
#define game_scr_x 2
#define menu_scr_h 3
#define menu_scr_w game_scr_w
#define menu_scr_y (game_scr_y + game_scr_h - menu_scr_h)
#define menu_scr_x game_scr_x
#define tile_size ((game_scr_h - 4 * PAD - menu_scr_h) / 11)
#define plr_h (tile_size + 2 * PAD)
#define plr_w game_scr_w
#define plr1_scr_h plr_h	// for makewin compatibility
#define plr1_scr_w plr_w	// for makewin compatibility
#define plr1_scr_y game_scr_y
#define plr1_scr_x game_scr_x
#define board_scr_h (9 * tile_size)
#define board_scr_w (9 * tile_size + 2 * PAD)
#define board_scr_y (plr1_scr_y + plr_h)
#define board_scr_x game_scr_x
#define plr2_scr_h plr_h	// for makewin compatibility
#define plr2_scr_w plr_w	// for makewin compatibility
#define plr2_scr_y (board_scr_y + board_scr_h)
#define plr2_scr_x game_scr_x
#define hud_scr_h board_scr_h
#define hud_scr_w (game_scr_w - board_scr_w)
#define hud_scr_y board_scr_y
#define hud_scr_x (board_scr_x + board_scr_w)
*/

// for static sizing
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
#define plr1_scr_h plr_h	// for makewin compatibility
#define plr1_scr_w plr_w	// for makewin compatibility
#define plr1_scr_y (game_scr_y + INNER_PAD_h)
#define plr1_scr_x (game_scr_x + INNER_PAD_w)
#define board_scr_h (9 * tile_size_h + 9 * TILE_PAD_h)
#define board_scr_w (9 * tile_size_w + 9 * TILE_PAD_w)
#define board_scr_y (plr1_scr_y + plr_h + INNER_PAD_h)
#define board_scr_x (game_scr_x + INNER_PAD_w)
#define plr2_scr_h plr_h	// for makewin compatibility
#define plr2_scr_w plr_w	// for makewin compatibility
#define plr2_scr_y (board_scr_y + board_scr_h + INNER_PAD_h)
#define plr2_scr_x plr1_scr_x
#define hud_scr_h board_scr_h
#define hud_scr_w (game_scr_w - board_scr_w - 3 * INNER_PAD_w)
#define hud_scr_y board_scr_y
#define hud_scr_x (board_scr_x + board_scr_w + INNER_PAD_w)


#endif


void	init_game	();


#endif
