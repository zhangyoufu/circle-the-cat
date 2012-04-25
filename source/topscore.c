#include "global.h"

#define SCOREBOARD_WIDTH     300
#define SCOREBOARD_HEIGHT    500
#define SCOREBOARD_X_BASE    ( SCREEN_WIDTH - SCOREBOARD_WIDTH ) / 2
#define SCOREBOARD_Y_BASE    ( SCREEN_HEIGHT - SCOREBOARD_HEIGHT ) / 2

static SDL_Surface* scoreboard;

bool is_top_score( void )
{
	return true;
}

static void scorewall_load( void )
{
    SDL_Surface* scorewall;
	scorewall = load_resource_for_RGBA( "scorewall.png" );
	SDL_BlitSurface( scorewall,
			         NULL,
			         scoreboard,
			         NULL );
}

static void button_load( void )
{
	SDL_Rect position = { 100, 410, 0, 0 };
	SDL_Surface* button;
    button = load_resource_for_RGBA( "button0.png" );
	SDL_BlitSurface( button,
			         NULL,
			         scoreboard,
			         &position );
}

static void inputbox_load( void )
{
	SDL_Rect position = { 50, 350, 0, 0 };
	SDL_Surface* inputbox;
	inputbox = load_resource_for_RGBA( "inputbox.png" );
    SDL_BlitSurface( inputbox,
			         NULL,
			         scoreboard,
			         &position );
}

static void load( void )
{
	scorewall_load( );
	button_load( );
	inputbox_load( );
}

static void unload( void )
{
}

static void scoreboard_initialize( void )
{
	scoreboard = create_surface( SCOREBOARD_WIDTH, SCOREBOARD_HEIGHT, 32, SDL_HWSURFACE );
	SDL_FillRect( scoreboard, NULL, TRANSPARENT_COLOR );
	SDL_BlitSurface( scoreboard,
			         NULL,
			         screen,
			         NULL );
    SDL_Flip( screen );
}

static SDL_Rect board_base = { SCOREBOARD_X_BASE, SCOREBOARD_Y_BASE, 0, 0 };

static void initialize( SDL_Surface *surface )
{
	Uint32 bgcolor = SDL_MapRGB( surface->format, 0xff, 0xff, 0xff );
	SDL_FillRect( surface, NULL, bgcolor );
	scoreboard_initialize( );
	SDL_BlitSurface( scoreboard, NULL, surface, &board_base );
}

static void finalize( void )
{
	SDL_FreeSurface( scoreboard );
}

static ViewDescriptor main_loop( void )
{	
	delay(5000);//waiting
	return VIEW_DESCRIPTOR( GAME_VIEW, NO_EFFECT );
}

View top_score_view = { load, initialize, main_loop, finalize, unload };
