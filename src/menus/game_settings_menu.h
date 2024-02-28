#ifndef GAME_SETTINGS_MENU_H
#define GAME_SETTINGS_MENU_H

#include "../utils/common.h"
#include "../core/game.h"	// game_settings_t and enum game_mode_t

#define game_settings_menu_scr_h 13
#define game_settings_menu_scr_w htow(game_settings_menu_scr_h)
#define game_settings_menu_scr_y ((term_h - game_settings_menu_scr_h) / 2)	// center of screen
#define game_settings_menu_scr_x ((term_w - game_settings_menu_scr_w) / 2)	// center of screen


enum return_option_t	init_human_game_settings_menu	(game_settings_t *game_settings);
enum return_option_t	init_ai_game_settings_menu		(game_settings_t *game_settings);

#endif
