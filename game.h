#ifndef GAME_H
#define GAME_H

// without boxes
// #define initialize_with_macro(x)	{x = newwin(x##_h, x##_w, x##_y, x##_x); keypad(x, TRUE);}
// #define translate_with_macro(x)		{wresize(x, x##_h, x##_w); mvwin(x, x##_y, x##_x); wclear(x);}

// with boxes
#define initialize_with_macro(x)	{x = newwin(x##_h, x##_w, x##_y, x##_x); keypad(x, TRUE); box(x, 0, 0);}
#define translate_with_macro(x)		{wresize(x, x##_h, x##_w); mvwin(x, x##_y, x##_x); wclear(x); box(x, 0, 0);}

#ifndef TRUE
#define TRUE 1
#endif
#ifndef FALSE
#define FALSE 0
#endif


#ifndef MAIN_MENU_H

#define PAD 2
#define game_h (term_h - 2 * PAD)
#define game_w (term_w - 2 * PAD)
#define game_y 2
#define game_x 2
#define menu_bar_h 3
#define menu_bar_w game_w
#define menu_bar_y (game_y + game_h - menu_bar_h)
#define menu_bar_x game_x
#define tile_size ((game_h - 4 * PAD - menu_bar_h) / 11)
#define plr_h (tile_size + 2 * PAD)
#define plr_w game_w
#define plr1_h plr_h	// for makewin compatibility
#define plr1_w plr_w	// for makewin compatibility
#define plr1_y game_y
#define plr1_x game_x
#define board_h (9 * tile_size)
#define board_w (9 * tile_size + 2 * PAD)
#define board_y (plr1_y + plr_h)
#define board_x game_x
#define plr2_h plr_h	// for makewin compatibility
#define plr2_w plr_w	// for makewin compatibility
#define plr2_y (board_y + board_h)
#define plr2_x game_x
#define hud_h board_h
#define hud_w (game_w - board_w)
#define hud_y board_y
#define hud_x (board_x + board_w)

#endif


#include <ncurses.h>

void init_game(void);


#endif
