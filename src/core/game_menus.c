#include <ncurses.h>

#include "game_menus.h"
#include "../main.h"	// for init macors


face_t show_promote_menu (const color_t color) {
	WINDOW *promote_menu_scr;
	initialize_with_box_at_center(promote_menu_scr);

	const NO_OF_OPTS = PIECE_TYPES - 2;
	unsigned int options[NO_OF_OPTS];
	unsigned int selected_opt = 0;
	for (unsigned int i = 0; i < PIECE_TYPES; i++) {
		if (!((1 << i) & KING || (1 << i) & PAWN))
			options[selected_opt++] = i;
	}
	selected_opt = 0;
	const int V_OFFSET = 1, H_OFFSET = 1;

	int key = -1;
	while (true) {
		if (key == KEY_RESIZE) {
			translate_with_box_at_center(promote_menu_scr);
			continue;
		}

		if (key == KEY_LEFT || key == 'h') {
			selected_opt = (selected_opt + NO_OF_OPTS - 1) % NO_OF_OPTS;
		} else if (key == KEY_RIGHT || key == 'l') {
			selected_opt = (selected_opt + 1) % NO_OF_OPTS;
		} else if (key == KEY_ENTER || key == '\r' || key == '\n') {
			break;
		}

		for (int i = 0; i < NO_OF_OPTS; i++) {
			if (selected_opt == i)
				wattron(promote_menu_scr, A_STANDOUT);
			wchar_t wstr[2];
			swprintf(wstr, 2, L"%lc", PIECES[SETTINGS_UNICODE_MODE][color][options[i]]);
			mvwaddnwstr(promote_menu_scr, V_OFFSET, H_OFFSET + i, wstr, 2);
			wattroff(promote_menu_scr, A_STANDOUT);
		}

		key = wgetch(promote_menu_scr);
	}

	wclear(promote_menu_scr);
	wrefresh(promote_menu_scr);
	delwin(promote_menu_scr);

	return (1 << (options[selected_opt]));
}
