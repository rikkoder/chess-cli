#ifndef GAME_MAENU_H
#define GAME_MENU_H

#include "board.h"


#define	promote_menu_scr_h	3
#define	promote_menu_scr_w	((PIECE_TYPES - 2) + 2)	// don't display PAWN and KING, but display borders


face_t	show_promote_menu	(const color_t color);

#endif
