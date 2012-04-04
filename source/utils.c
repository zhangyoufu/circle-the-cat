#include <stdlib.h>
#include "global.h"

/*
 * Create a surface
 * Note: only accept 24 or 32bpp
 */
SDL_Surface* create_surface( int width, int height, int bpp, Uint32 flags )
{
	return SDL_CreateRGBSurface( flags,
	                             width,
	                             height,
	                             bpp,
	                             0x00ff0000,
	                             0x0000ff00,
	                             0x000000ff,
	                             bpp == 32 ? 0xff000000 : 0x00000000 );
}

/*
 * Create a surface the same size as screen
 * Note: exactly 32bit RGB
 */
SDL_Surface* create_screen_size_surface( Uint32 flags )
{
	return SDL_CreateRGBSurface( flags,
	                             screen->w,
	                             screen->h,
	                             screen->format->BitsPerPixel,
	                             screen->format->Rmask,
	                             screen->format->Gmask,
	                             screen->format->Bmask,
	                             screen->format->Amask );
}

#define BACKGROUND_COLOR	SDL_MapRGB( screen->format, 0xff, 0xff, 0xff )

/* Fill the surface with BACKGROUND_COLOR */
void apply_background( SDL_Surface *surface )
{
	SDL_FillRect( surface, NULL, BACKGROUND_COLOR );
}

/*
 * Return the pixel value at (x, y)
 * NOTE: The surface must be locked before calling this!
 *
 * URL: http://www.libsdl.org/cgi/docwiki.cgi/Introduction_to_SDL_Video#getpixel
 */
Uint32 get_pixel( SDL_Surface *surface, int x, int y )
{
    int bpp = surface->format->BytesPerPixel;
    /* Here p is the address to the pixel we want to retrieve */
    Uint8 *p = (Uint8 *)surface->pixels + y * surface->pitch + x * bpp;
	Uint32 result = 0;

	SDL_LockSurface( surface );
    switch (bpp)
	{
		case 1:
			result = *p;

		case 2:
			result = *(Uint16 *)p;

		case 3:
			if (SDL_BYTEORDER == SDL_BIG_ENDIAN)
				result = p[0] << 16 | p[1] << 8 | p[2];
			else
				result = p[0] | p[1] << 8 | p[2] << 16;

		case 4:
			result = *(Uint32 *)p;
    }
	SDL_UnlockSurface( surface );
	return result;
}

#ifndef MAX_PATH
#define MAX_PATH	128
#endif

/* TODO: Use integrated resources */
/* Load image in 'resource' directory */
SDL_Surface* load_resource( const char *name )
{
	char path[MAX_PATH];
	
	_snprintf( path, sizeof path, "resource/%s", name );
	return IMG_Load( path );
}

/*
 * Load transparent resource used in RGBA -> RGBA blits
 * clear SDL_SRCALPHA, set SDL_SRCCOLORKEY
 */
SDL_Surface* load_resource_for_RGBA( const char *name )
{
	SDL_Surface* surface = load_resource( name );

	SDL_SetAlpha( surface, 0, SDL_ALPHA_OPAQUE );
	SDL_SetColorKey( surface, SDL_SRCCOLORKEY, TRANSPARENT_COLOR );

	return surface;
}

/* Check SDL_QUIT event while switching between views */
void check_quit_event( void )
{
	SDL_Event event;
	
	while( SDL_PollEvent( &event ) )
		if( event.type == SDL_QUIT )
			exit( EXIT_SUCCESS );
}

/* Wait for input events, used by view */
void get_input_event( SDL_Event *event )
{
	while( SDL_WaitEvent( event ) )
		switch( event->type )
		{
			case SDL_MOUSEMOTION:
			case SDL_MOUSEBUTTONDOWN:
			case SDL_MOUSEBUTTONUP:
			case SDL_KEYDOWN:
			case SDL_KEYUP:
				/* Accepted */
				return;
			
			case SDL_QUIT:
				exit( EXIT_SUCCESS );
		}
}

/* The same granularity as SDL_WaitEvent */
#define DELAY_GRANULARITY	10

/* Delay(ms), check for SDL_QUIT event periodically */
void delay( int time )
{
	Uint32 end_tick = SDL_GetTicks( ) + time - DELAY_GRANULARITY/2;
	SDL_Event event;
	
	while( SDL_GetTicks( ) < end_tick )
	{
		while( SDL_PollEvent( &event ) )
			if( event.type == SDL_QUIT )
				exit( EXIT_SUCCESS );
		
		SDL_Delay( DELAY_GRANULARITY );
	}
}
