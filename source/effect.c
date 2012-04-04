#include "global.h"

static void dummy( SDL_Surface *from, SDL_Surface *to )
{
	UNUSED( from );
	UNUSED( to );
}

static void apply_alpha( SDL_Surface *foreground, Uint8 alpha )
{
	/* Apply background to screen */
	apply_background( screen );

	/* Set alpha for foreground surface */
	SDL_SetAlpha( foreground, SDL_SRCALPHA, alpha );
	SDL_BlitSurface( foreground, NULL, screen, NULL );

	/* Update screen */
	SDL_Flip( screen ); 
}

#define FADE_DELAY			1000
#define FADE_IN_STEP		3
#define FADE_OUT_STEP		1

static void fade_switch( SDL_Surface *from, SDL_Surface *to )
{
	int i;

    if( from )
	{
		/* Fade out */
		for( i = 255; i > 0; i-=FADE_OUT_STEP ) {
			check_quit_event( );
			apply_alpha( from, (Uint8)i );
		}
		
		/* TODO: do we need to put some delay here? */
    }
	
	apply_alpha( to, 0 );
	delay( FADE_DELAY );

	/* Fade in */
	for( i = FADE_IN_STEP; i < 255; i+=FADE_IN_STEP ) {
		check_quit_event( );
		apply_alpha( to, (Uint8)i );
	}
}

Effect no_effect = {
	dummy
};

Effect fade_effect = {
	fade_switch
};
