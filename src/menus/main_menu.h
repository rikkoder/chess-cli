#ifndef MAIN_MENU_H
#define MAIN_MENU_H

#include "../utils/common.h"

#define main_menu_scr_h 12
#define main_menu_scr_w htow(main_menu_scr_h)
#define main_menu_scr_y ((term_h - main_menu_scr_h) / 2)	// center of screen
#define main_menu_scr_x ((term_w - main_menu_scr_w) / 2)	// center of screen


void init_main_menu	(void);

#endif
