#include "global.h"
#include "board.h"
#include <string.h>

/*******************************************************************************
 * Global variable
 ******************************************************************************/
/* Store board state */
Cell cell[ROWS][COLS];

/*******************************************************************************
 * Static variable used by more than one function
 ******************************************************************************/
static SDL_Surface* board;
static SDL_Surface* cell_tile[CELL_TYPE_MAX];
static const int cell_x_base[2] = { 0, CELL_X_DELTA / 2 };

/*******************************************************************************
 * Draw
 ******************************************************************************/
/* Draw the board */
static void draw_board( void )
{
	int i, j, x, y;

	SDL_FillRect( board, NULL, TRANSPARENT_COLOR );
	y = 0;
	for( i = 0; i < ROWS; i++ )
	{
		x = cell_x_base[ i & 1 ];
		for( j = 0; j < COLS; j++ )
		{
			SDL_Rect position = { x, y, 0, 0 };

			SDL_BlitSurface( cell_tile[ cell[i][j].type ],
			                 NULL,
			                 board,
			                 &position );
			x += CELL_X_DELTA;
		}
		y += CELL_Y_DELTA;
	}
}

/* Board position in screen */
static SDL_Rect board_base = { BOARD_X_BASE, BOARD_Y_BASE, 0, 0 };

/* Draw board on specified surface */
void apply_board( SDL_Surface *surface )
{
	SDL_BlitSurface( board, NULL, surface, &board_base );
}

/* Update the board surface */
void board_update( int row, int col )
{
	SDL_Rect position;
	int x, y;
	
	calc_cell_position( row, col, &x, &y );

	position.x = x;
	position.y = y;
	position.w = position.h = 0;
	SDL_BlitSurface( cell_tile[ CELL_BARRIER ],
	                 0,
	                 board,
	                 &position );
}

/*******************************************************************************
 * Caculate
 ******************************************************************************/
/* Calculate cell's coordinate(left-up corner) according to row and column */
void calc_cell_position( int row, int col, int *x, int *y )
{
	*x = cell_x_base[ row & 1 ] + CELL_X_DELTA * col;
	*y = CELL_Y_DELTA * row;
}

/* Cell position deltas {even, odd} */
static int row_delta[2][DIRECTION_MAX] = {
	{  0, -1,  0, +1, +1,  0, -1 },
	{  0, -1,  0, +1, +1,  0, -1 }
};
static int col_delta[2][DIRECTION_MAX] = {
	{  0,  0, +1,  0, -1, -1, -1 },
	{  0, +1, +1, +1,  0, -1,  0 }
};

/* Calculate cell position in the direction specified, then write back */
bool calc_cell_move( int *row, int *col, Direction dir )
{
	int index = cat.row % 2;
	int new_row = *row + row_delta[index][dir];
	int new_col = *col + col_delta[index][dir];
	
	if( new_row < 0 || new_row >= ROWS || new_col < 0 || new_col >= COLS )
		return false;
	*row = new_row; *col = new_col;
	return true;
}

static const int x_delta_divisor[DIRECTION_MAX] =
	{  1,  2,  1,  2, -2, -1, -2 };
static const int y_delta_multiplier[DIRECTION_MAX] =
	{  0, -1,  0,  1,  1,  0, -1 };

void calc_position_move( int *x, int *y, Direction direction )
{
	*x += CELL_X_DELTA / x_delta_divisor[ direction ];
	*y += CELL_Y_DELTA * y_delta_multiplier[ direction ];
}

/*******************************************************************************
 * Check
 ******************************************************************************/
static bool in_board( int x, int y )
{
	if( x < BOARD_X_BASE || y < BOARD_Y_BASE )
		return false;
	
	if( x >= BOARD_X_BASE + BOARD_WIDTH || y >= BOARD_Y_BASE + BOARD_HEIGHT )
		return false;

	return true;
}

/*******************************************************************************
 * Match
 ******************************************************************************/
#define PATTERN_PART_COUNT		4
#define PATTERN_WIDTH			CELL_X_DELTA
#define PATTERN_HEIGHT			( CELL_Y_DELTA * 2 )

/* Note: The central part has an offset of (0,0) */
static Direction part_direction[PATTERN_PART_COUNT] = {
	LEFT_UP, RIGHT_UP, LEFT_DOWN, RIGHT_DOWN
};
static const int part_row_offset[PATTERN_PART_COUNT] = { -1, -1, +1, +1 };
static const int part_col_offset[PATTERN_PART_COUNT] = { -1,  0, -1,  0 };

static int row_offset_pattern[PATTERN_WIDTH][PATTERN_HEIGHT];
static int col_offset_pattern[PATTERN_WIDTH][PATTERN_HEIGHT];

static void generate_offset_pattern( void )
{
	SDL_Surface *target, *source;
	int i;

	source = cell_tile[ CELL_GROUND ];
	target = create_surface( PATTERN_WIDTH, PATTERN_HEIGHT, 32, SDL_SWSURFACE );
	memset( row_offset_pattern, 0, sizeof row_offset_pattern );
	memset( col_offset_pattern, 0, sizeof col_offset_pattern );

	for( i = 0; i < PATTERN_PART_COUNT; i++ )
	{
		SDL_Rect position;
		int x = 0, y = 0;

		SDL_FillRect( target, NULL, TRANSPARENT_COLOR );

		calc_position_move( &x, &y, part_direction[i] );

		position.x = x;
		position.y = y;
		position.w = position.h = 0;
		SDL_BlitSurface( source, NULL, target, &position );
		
		SDL_LockSurface( target );
		for( x = 0; x < PATTERN_WIDTH; x++ )
			for( y = 0; y < PATTERN_HEIGHT; y++ )
				if( get_pixel( target, x, y ) != TRANSPARENT_COLOR )
				{
					row_offset_pattern[x][y] = part_row_offset[i];
					col_offset_pattern[x][y] = part_col_offset[i];
				}
		SDL_UnlockSurface( target );
	}
	
	SDL_FreeSurface( target );
}

bool find_cell( int x, int y, int *row, int *col )
{
	bool result = false;
	int _row, _col;
	
	if( !in_board( x, y ) )
		return false;

	x -= BOARD_X_BASE;
	y -= BOARD_Y_BASE;

	SDL_LockSurface( board );
	
	if( get_pixel( board, x, y ) == CELL_GROUND_COLOR )
	{
		_col = x / PATTERN_WIDTH;      x %= PATTERN_WIDTH;
		_row = y / PATTERN_HEIGHT * 2; y %= PATTERN_HEIGHT;

//		printf( "row=%d+(%d), col=%d+(%d), y=%d, x=%d\n",
//				_row, row_offset_pattern[x][y],
//				_col, col_offset_pattern[x][y],
//				y, x );

		_col += col_offset_pattern[x][y];
		_row += row_offset_pattern[x][y];
		
		result = _row != cat.row || _col != cat.col;
		
		if( row && col )
		{
			*row = _row;
			*col = _col;
		}
	}
	
	SDL_UnlockSurface( board );

	return result;
}

/*******************************************************************************
 * Interface
 ******************************************************************************/
/* Load resource */
void board_load( void )
{
	cell_tile[ CELL_GROUND ]  = load_resource_for_RGBA( "cell_ground.png" );
	cell_tile[ CELL_BARRIER ] = load_resource_for_RGBA( "cell_barrier.png" );

	generate_offset_pattern( );
}

/* Initialize */
void board_initialize( void )
{
	board = create_surface( BOARD_WIDTH, BOARD_HEIGHT, 32, SDL_HWSURFACE );
	draw_board( );
}

/* Finalize */
void board_finalize( void )
{
	SDL_FreeSurface( board );
}
