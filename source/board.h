#ifndef BOARD_H
#define BOARD_H

#pragma once

#define CELL_WIDTH			48
#define CELL_HEIGHT			44
#define CELL_X_INTERVAL		5
#define CELL_X_DELTA		( CELL_WIDTH + CELL_X_INTERVAL )
#define CELL_Y_DELTA		41
#define CELL_GROUND_COLOR	SDL_MapRGB( board->format, 0xcc, 0xff, 0x00 )

#define BOARD_WIDTH		( CELL_X_DELTA*COLS + (CELL_WIDTH-CELL_X_INTERVAL) / 2 )
#define BOARD_HEIGHT	( CELL_Y_DELTA*(ROWS-1) + CELL_HEIGHT )
#define BOARD_X_BASE	( SCREEN_WIDTH - BOARD_WIDTH ) / 2
#define BOARD_Y_BASE	( SCREEN_HEIGHT - BOARD_HEIGHT ) / 2

#endif /* BOARD_H */