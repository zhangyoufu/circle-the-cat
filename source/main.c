#include <stdlib.h>
#include <time.h>
#include "global.h"
#include <windows.h>
#include <io.h>
#include <fcntl.h>

SDL_Surface *screen;

/* Filter out Alt-F4 keydown event, then send a SDL_QUIT event */
int event_filter( const SDL_Event *event )
{
	if( event->type == SDL_KEYDOWN || event->type == SDL_KEYUP )
	{
		SDLKey key = event->key.keysym.sym;
		SDLMod mod = event->key.keysym.mod;
		
		if( key == SDLK_F4 && ( mod == KMOD_LALT ||
								mod == KMOD_RALT ) )
		{
			SDL_Event quit_event = { SDL_QUIT };
			
			SDL_PushEvent( &quit_event );
			return 0;
		}
	}
	
	return 1;
}

/* 
void fatal( const char *msg )
{
	fprintf( stderr, "%s\n", msg );
	exit( EXIT_FAILURE );
}
*/

/* Wrapped by SDL as SDL_main */
int main( int argc, char* argv[] )
{
	int		next_view;

	UNUSED( argc );
	UNUSED( argv );
	
	/* Random seed */
	srand( time( NULL ) );
	
	/* TODO: Add sound like mew~ */
	SDL_Init( SDL_INIT_TIMER | SDL_INIT_AUDIO | SDL_INIT_VIDEO );
	atexit( SDL_Quit );
	
	SDL_SetEventFilter( event_filter );
	
	view_initialize( );
	cursor_initialize( );
	set_cursor( CURSOR_ARROW );
	
	/* TODO: Try SDL_NOFRAME flag */
	screen = SDL_SetVideoMode( SCREEN_WIDTH,
	                           SCREEN_HEIGHT,
	                           0,
							   // SDL_SWSURFACE );
	                           SDL_HWSURFACE | SDL_DOUBLEBUF );
	
	next_view = VIEW_DESCRIPTOR( INITIAL_VIEW, FADE_EFFECT );
	while( next_view )
		next_view = show_view( next_view );
	
	// cursor_finalize( );
	return 0;
}
