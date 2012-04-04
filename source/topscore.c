#include "global.h"

bool is_top_score( void )
{
	return false;
}

static void load( void )
{
}

static void unload( void )
{
}

static void initialize( SDL_Surface *surface )
{
	Uint32 bgcolor = SDL_MapRGB( surface->format, 0xff, 0xff, 0xff );
	SDL_FillRect( surface, NULL, bgcolor );
}

static void finalize( void )
{
}

static ViewDescriptor main_loop( void )
{
	return VIEW_DESCRIPTOR( GAME_VIEW, NO_EFFECT );
}

View top_score_view = { load, initialize, main_loop, finalize, unload };
