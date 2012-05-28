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
		set_cursor( CURSOR_NORMAL );
}

static int check_cursor_for_cat( int x, int y, int row, int col )
{
	
}

/* Judge whether cat can move */
static bool search_move( int row, int col )
{
    bool movable = false;
	for( dir = 1 ; dir < MAX_DIRECTION ; dir++ )
	{
		if( can_move( row, col, dir) )
		movable = true;
		break;
	}
	return movable;
}

static void load_image_cat_turn( SDL_Surface *surface )
{
	SDL_Rect position = { 0, 80, 0, 0 };
	SDL_Surface* image_resource = load_resource_for_RGBA( "cat_turn.gif" );
	SDL_BlitSurface( image_resource,
	                 NULL,
	                 surface,
	                 &position );
    SDL_FreeSurface( image_resource );
    if( surface == screen )
		SDL_Flip( screen );
}

static void load_image_barrier_turn( SDL_Surface *surface )
{
	SDL_Rect position = { 700, 80, 0, 0 };
	SDL_Surface* image_resource = load_resource_for_RGBA( "barrier_turn.gif" );
	SDL_BlitSurface( image_resource,
	                 NULL,
	                 surface,
	                 &position );
	SDL_FreeSurface( image_resource );
    if( surface == screen )
		SDL_Flip( screen );
}

static void load_image_cat_win( SDL_Surface *surface )
{
	SDL_Rect position = { , , 0, 0 };
	SDL_Surface* image_resource = load_resource_for_RGBA( "cat_win.png" );
	SDL_BlitSurface( image_resource,
	                 NULL,
	                 surface,
	                 &position );
	SDL_FreeSurface( image_resource );
    if( surface == screen )
		SDL_Flip( screen );
}

static void load_image_barrier_win( SDL_Surface *surface )
{
	SDL_Rect position = { , , 0, 0 };
	SDL_Surface* image_resource = load_resource_for_RGBA( "barrier_win.png" );
	SDL_BlitSurface( image_resource,
	                 NULL,
	                 surface,
	                 &position );
	SDL_FreeSurface( image_resource );
    if( surface == screen )
		SDL_Flip( screen );
}

/* Redraw all items */
//void draw_all( SDL_Surface *surface, int frame )
//{
//	apply_background( surface );
//	apply_board( surface );
//	draw_cat( surface, frame );
//	if( surface == screen )
//		SDL_Flip( screen );
//}

/******************************************************************************/

static void load( void )
{
	board_load( );
	cat_load( );
}

static void initialize( SDL_Surface *surface )
{
	memset( cell, 0, sizeof cell );
	board_initialize( );
	cat_initialize( );
	computer_initialize( );

	draw_all( surface, 0 );
}

static void finalize( void )
{
	board_finalize( );
}

#define GAME_OVER_CAT_STILL_DELAY		2500
#define GAME_OVER_CAT_WIN_DELAY			1500

/* Event loop */
static ViewDescriptor main_loop( void )
{
	bool game_over = false, cat_turn = false;
	int x, y;
	
	if( SDL_GetMouseState( &x, &y ) & SDL_BUTTON_LEFT )
		set_cursor( CURSOR_NORMAL );
	else
		check_and_set_cursor( x, y );
		
	/* Print image Barrier's Turn */
	load_image_barrier_turn( );
	
	while( !game_over )
	{
		SDL_Event	event;
		int			row, col;
		int         direction = UNABLE_TO_MOVE;
		
		get_input_event( &event );
		if( cat_turn )/* Cat's turn */
		{
		    switch( event.type )
			{
				case SDL_MOUSEMOTION:
				    /* Prohibit mouse event until left button is up */
					if( event.motion.state & SDL_BUTTON_LEFT )
						break;
					/* Check the direction of cat's next move */
					direction = check_cursor_for_cat( event.motion.x, event.motion.y, row, col );
				    break;
                case SDL_MOUSEBUTTONDOWN:
					/* Left button only */
					if( event.button.button != SDL_BUTTON_LEFT )
						break;
					if( direction != UNABLE_TO_MOVE )/* Move to an available direction */
					{
						set_cursor( CURSOR_NORMAL );
						/* Cat moves */
						cat.direction = direction;
						if( cat_move( ) )
						{
							draw_all( screen, 0 );
							
							/* Change to barrier's turn */
							cat_turn = false;
							load_image_barrier_turn( screen );

							/* Reset direction */
							direction = UNABLE_TO_MOVE;  
						}
						else
						{
							/* TODO: sound effect */
							while( cat_in_screen( ) )
								cat_move( );
							/* Print image Cat Win */
							load_image_cat_win( screen );
							delay( GAME_OVER_CAT_WIN_DELAY );
							game_over = true;
						}
			        }
			        else/* Illegal move */
			        {
        				/* TODO: sound effect */
        			}
					break;
                case SDL_KEYDOWN:
		            if( event.key.keysym.sym == SDL_ESCAPE )
		                game_over = true;
                    break;				
			}
		}
		else/* barrier's turn */
		{
			switch( event.type )
			{
				case SDL_MOUSEMOTION:
					/* Prohibit mouse event until left button is up */
					if( event.motion.state & SDL_BUTTON_LEFT )
						break;
	
					check_and_set_cursor( event.motion.x, event.motion.y );
					break;
			
				case SDL_MOUSEBUTTONDOWN:
					/* Left button only */
					if( event.button.button != SDL_BUTTON_LEFT )
						break;					
					if( find_cell( event.button.x, event.button.y, &row, &col ) )
					{
						if( cell[row][col].type == CELL_GROUND )
						{
							set_cursor( CURSOR_NORMAL );

							/* Set the cell to barrier */
							cell[row][col].type = CELL_BARRIER;
							board_update( row, col );

							/* Judge whether barrier has win */
							if( !search_move( row, col ) )/* Barrier has won */
							{
								/* TODO: sound effect */
								draw_all( screen, 1 );
								/* Print image Barrier Win */
								load_image_barrier_win( screen );
								delay( GAME_OVER_CAT_STILL_DELAY );
								game_over = true;
							}
							else/* Barrier hasn't won */
							{
							    draw_all( screen, 0 );

								/* Change to cat's turn */
							    cat_turn = true;
							    load_image_cat_turn( screen );
							}
						}
						else /* cell[row][col].state == CELL_BARRIER */
						{
							/* TODO: sound effect */
						}
					}
					break;
				case SDL_KEYDOWN:
		            if( event.key.keysym.sym == SDL_ESCAPE )
		                game_over = true;
                    break;
			}
		}
		if( event.type == SDL_QUIT )
		    game_over = true;
	}
	
	return VIEW_DESCRIPTOR( GAME_VIEW, FADE_EFFECT );
}

View multi_view = { load, initialize, main_loop, finalize, NULL };
