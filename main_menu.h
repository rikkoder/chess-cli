#ifndef MAIN_MENU_H
#define MAIN_MENU_H

#include <ncurses.h>
#include <wchar.h>

#define main_menu_h 10
#define main_menu_w 20
#define main_menu_y ((term_h - main_menu_h) / 2)
#define main_menu_x ((term_w - main_menu_w) / 2)

void init_main_menu(void);

#endif
