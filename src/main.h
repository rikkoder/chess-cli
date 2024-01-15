#ifndef MAIN_H
#define MAIN_H

#include <ncurses.h>

#define htow(h) (2*h)

#define window_h 40
#define window_w htow(window_h)

// without boxes
#define initialize_without_box(x)	{x = newwin(x##_h, x##_w, x##_y, x##_x); keypad(x, true);}
#define translate_without_box(x)	{wresize(x, x##_h, x##_w); mvwin(x, x##_y, x##_x); wclear(x);}

// with boxes
#define initialize_with_box(x)	{x = newwin(x##_h, x##_w, x##_y, x##_x); keypad(x, true); box(x, 0, 0);}
#define translate_with_box(x)	{wresize(x, x##_h, x##_w); mvwin(x, x##_y, x##_x); wclear(x); box(x, 0, 0);}

// auto center
#define	initialize_with_box_at_center(x) {\
	int term_h, term_w;\
	getmaxyx(stdscr, term_h, term_w);\
	x = newwin(x##_h, x##_w, (term_h - x##_h)/2, (term_w - x##_w)/2);\
	keypad(x, true);\
	box(x, 0, 0);\
}
#define	translate_with_box_at_center(x) {\
	int term_h, term_w;\
	getmaxyx(stdscr, term_h, term_w);\
	wresize(x, x##_h, x##_w);\
	mvwin(x, (term_h - x##_h)/2, (term_w - x##_w)/2);\
	wclear(x);\
	box(x, 0, 0);\
}


#define SAVE_EXT		"hstk"
#define BASE_DIR		"chess-cli-files"
#define SAVE_DIR		".saves"
#define	PGN_DIR			"pgn-exports"
#define PGN_EXT			"pgn"

#endif
