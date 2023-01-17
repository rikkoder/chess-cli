#ifndef MAIN_MENU_H
#define MAIN_MENU_H

#include "main.h"

// for dynamic sizing
/* 
#define main_menu_h 10
#define main_menu_w 20
#define main_menu_y ((term_h - main_menu_h) / 2)
#define main_menu_x ((term_w - main_menu_w) / 2)
 */

// for static sizing
#define main_menu_h 10
#define main_menu_w htow(main_menu_h)
#define main_menu_y ((term_h - main_menu_h) / 2)	// center of screen
#define main_menu_x ((term_w - main_menu_w) / 2)	// center of screen


void init_main_menu();

#endif
