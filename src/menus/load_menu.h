#ifndef LOAD_MENU_H
#define LOAD_MENU_H

#include "../utils/common.h"
#include "../core/history.h"

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

#define	MAX_WARNING_SIZE	25

#define warning_scr_h 3
#define warning_scr_w MAX_WARNING_SIZE + 2
#define warning_scr_y ((term_h - warning_scr_h) / 2)	// center of screen
#define warning_scr_x ((term_w - warning_scr_w) / 2)	// center of screen

enum	load_return_code	{LOAD_FILE_SELECTED, LOAD_FILE_NOT_SELECTED};


enum load_return_code	load_menu			(timestamp_t load_timestamp);
void					show_warning_scr	(const char *msg);

#endif
