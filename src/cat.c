#include "global.h"
#include "board.h"

Cat cat;

#define FRAME_MAX			7

static SDL_Surface* cat_repo[DIRECTION_MAX][FRAME_MAX];

#define CAT_X_CENTER		62
#define CAT_Y_CENTER		72

/* Set cat's display position */
static void calc_cat_position( void )
{
	calc_cell_position( cat.row, cat.col, &cat.x, &cat.y );
	cat.x += BOARD_X_BASE; cat.x -= CAT_X_CENTER;
	cat.y += BOARD_Y_BASE; cat.y -= CAT_Y_CENTER;
}

/* Draw cat on specified surface */
void draw_cat( SDL_Surface *surface, int frame )
{
	SDL_Rect position = { cat.x, cat.y, 0, 0 };

	SDL_BlitSurface( cat_repo[cat.direction][frame],
	                 NULL,
	                 surface,
	                 &position );
}

#define CAT_MOVE_FRAME_INTERVAL		50

/* Move the cat */
bool cat_move( void )
{
	int i;
	
	for( i = 1; i < FRAME_MAX; i++ )
	{
		draw_all( screen, i );
		delay( CAT_MOVE_FRAME_INTERVAL );
	}
	
	if( calc_cell_move( &cat.row, &cat.col, cat.direction ) )
	{
		/* Calibrate cat's coordinate according to its cell position */
		calc_cat_position( );
		return true;
	}
	else
	{
		/* Out of board, change display position only */
		calc_position_move( &cat.x, &cat.y, cat.direction );
		return false;
	}
}

#define CAT_WIDTH		172
#define CAT_HEIGHT		149

/* Check whether the cat has run out of screen */
bool cat_in_screen( void )
{
	if( cat.x + CAT_WIDTH  <= 0 || cat.x >= SCREEN_WIDTH ||
	    cat.y + CAT_HEIGHT <= 0 || cat.y >= SCREEN_HEIGHT )
		return false;
	return true;
}

void cat_load( void )
{
	int i, j;
	
	for( i = 1; i < DIRECTION_MAX; i++ )
	{
		for( j = 0; j < 5; j++ )
		{
			char buf[8];
			
			_snprintf( buf, sizeof buf, "%d-%d.png", i, j );
			cat_repo[i][j] = load_resource( buf );
		}
		cat_repo[i][5] = cat_repo[i][6] = cat_repo[i][4];
		cat_repo[i][4] = cat_repo[i][3];
	}	
}

void cat_initialize( void )
{
	cat.row = ROWS / 2;
	cat.col = COLS / 2;
	calc_cat_position( );
	cat.direction = LEFT_DOWN;
}
