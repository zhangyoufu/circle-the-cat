#include "global.h"
#include "board.h"

/* Check clickable hot ares */
static bool check_clickable( int x, int y )
{
	return find_cell( x, y, NULL, NULL );
}

static void check_and_set_cursor( int x, int y )
{
	/* TODO: change cursor style for click area */
	if( check_clickable( x, y ) )
		set_cursor( CURSOR_HAND );
	else
		set_cursor( CURSOR_ARROW );
}

/* Redraw all items */
void draw_all( SDL_Surface *surface, int frame )
{
	apply_background( surface );
	apply_board( surface );
	draw_cat( surface, frame );
	if( surface == screen )
		SDL_Flip( screen );
}

/******************************************************************************/

/* Load resources */
static void load( void )
{
	board_load( );
	cat_load( );
}

/* Initialize */
static void initialize( SDL_Surface *surface )
{
	memset( cell, 0, sizeof cell );
	board_initialize( );
	cat_initialize( );
	computer_initialize( );

	draw_all( surface, 0 );
}

/* Finalize */
static void finalize( void )
{
	board_finalize( );
}

#define GAME_OVER_CAT_STILL_DELAY		2300
#define GAME_OVER_CAT_WIN_DELAY			1500

/* Event loop */
static ViewDescriptor main_loop( void )
{
	bool game_over = false, cat_win = false;
	int x, y;
	
	if( SDL_GetMouseState( &x, &y ) & SDL_BUTTON_LEFT )
		set_cursor( CURSOR_ARROW );
	else
		check_and_set_cursor( x, y );
	
	while( !game_over )
	{
		SDL_Event	event;
		int			row, col;
		
		get_input_event( &event );
		switch( event.type )
		{
			case SDL_MOUSEMOTION:
			{
				/* Prohibit mouse event until left button is up */
				if( event.motion.state & SDL_BUTTON_LEFT )
					break;
	
				check_and_set_cursor( event.motion.x, event.motion.y );
				break;
			}
			
			case SDL_MOUSEBUTTONDOWN:
			{
				/* Left button only */
				if( event.button.button != SDL_BUTTON_LEFT )
					break;
				
				/* TODO: restore default cursor */
				/* TODO: UI buttons */
				
				if( find_cell( event.button.x, event.button.y, &row, &col ) )
				{
//					printf( "button(%d,%d) -> cell(%d,%d)\n",
//					        event.button.x, event.button.y,
//					        row, col );

					if( cell[row][col].type == CELL_GROUND )
					{
						int direction;
						
						/* Set the cell to barrier */
						cell[row][col].type = CELL_BARRIER;
						board_update_cell( row, col );

						direction = computer_decision( row, col );
						if( direction == UNABLE_TO_MOVE )
						{
							/* Render the first frame of animation in last move
							 * direction, and then delay for a while to make the
							 * cat still */
							draw_all( screen, 1 );
							delay( GAME_OVER_CAT_STILL_DELAY );
							game_over = true;
						}
						else /* direction != UNABLE_TO_MOVE */
						{
							cat.direction = direction;
							if( cat_move( ) )
							{
								draw_all( screen, 0 ); /* sit there */
							}
							else
							{
								while( cat_in_screen( ) )
									cat_move( );
								delay( GAME_OVER_CAT_WIN_DELAY );
								cat_win = true;
								game_over = true;
							}
						}
						
						set_cursor( CURSOR_ARROW );
					}
					else /* cell[row][col].state == CELL_BARRIER */
					{
						/* TODO: sound effect */
					}
				}
				break;
			}
		}
	}
	
	if( cat_win && is_top_score() )
		return VIEW_DESCRIPTOR( TOP_SCORE_VIEW, FADE_EFFECT );
	return VIEW_DESCRIPTOR( GAME_VIEW, FADE_EFFECT );
}

View game_view = { load, initialize, main_loop, finalize, NULL };
