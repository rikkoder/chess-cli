#ifndef BOARD_H
#define BOARD_H

#include <wchar.h>
#include <stdint.h>
#include <stdbool.h>

#define ASCII		0
#define UNICODE		(1 << 7)

#define WHITE		0
#define BLACK		(1 << 6)
#define COLOR_BIT	(1 << 6)

#define KING		(1 << 0)
#define QUEEN		(1 << 1)
#define ROOK		(1 << 2)
#define BISHOP		(1 << 3)
#define KNIGHT		(1 << 4)
#define PAWN		(1 << 5)

#define NO_PIECE 0
#define INVALID_ROW -1
#define INVALID_COL -1

typedef uint32_t		chance_t;
typedef uint32_t		face_t;
typedef bool			color_t;
typedef struct tile_t	tile_t;

enum result { STALE_MATE, WHITE_WON, BLACK_WON, PENDING };

typedef struct {
	face_t face;
	bool is_moved;
} piece_t ;

typedef struct tile_t {
	short row;
	short col;
	piece_t *piece;
	bool can_be_dest;
	bool has_check[2];
} tile_t;

typedef struct board_t {
	tile_t tiles[8][8];
	tile_t *kings[2];
	chance_t chance;
	enum result result;
} board_t;


piece_t*	init_piece		(short i, short j);
wchar_t		get_piece_face	(const piece_t *piece);
void		init_board		(board_t *board);
void		copy_board		(board_t *dest_board, const board_t *src_board);
void		delete_board	(board_t *board);


#endif
