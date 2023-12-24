#ifndef MAIN_MENU_H
#define MAIN_MENU_H

#include "main.h"

#define main_menu_scr_h 10
#define main_menu_scr_w htow(main_menu_scr_h)
#define main_menu_scr_y ((term_h - main_menu_scr_h) / 2)	// center of screen
#define main_menu_scr_x ((term_w - main_menu_scr_w) / 2)	// center of screen
#define load_menu_scr_h 20
#define load_menu_scr_w htow(load_menu_scr_h)
#define load_menu_scr_y ((term_h - load_menu_scr_h) / 2)	// center of screen
#define load_menu_scr_x ((term_w - load_menu_scr_w) / 2)	// center of screen

#define	LOAD_PAGE_SIZE			30
#define	LOAD_LIST_SIZE			15
#define	TIMESTAMP_DISPLY_SIZE	15	//HH:MM DD MON YY
#define	NO_OF_LOAD_LISTS		(LOAD_PAGE_SIZE/LOAD_LIST_SIZE)
#define	LOAD_LIST_PAD			((load_menu_scr_w - 2 - (NO_OF_LOAD_LISTS * TIMESTAMP_DISPLY_SIZE))/(NO_OF_LOAD_LISTS+1))
#define LOAD_EXTRA_PAD			(1 + (((load_menu_scr_w - 2 - (NO_OF_LOAD_LISTS * TIMESTAMP_DISPLY_SIZE))%(NO_OF_LOAD_LISTS+1))/2))



void init_main_menu	(void);

#endif
