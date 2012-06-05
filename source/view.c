#include "global.h"

View* view_repo[VIEW_MAX] = {
	NULL, /* quit */
	&game_view,
	&top_score_view,
};

Effect* effect_repo[EFFECT_MAX] = {
	&no_effect,
	&fade_effect,
};

int show_view( ViewDescriptor desc )
{
	static bool initial_view = true;
	SDL_Surface *from, *to;
	View *view;
	Effect *effect;
	
	view = view_repo[ EXTRACT_VIEW(desc) ];
	effect = effect_repo[ EXTRACT_EFFECT(desc) ];
	
	if( initial_view )
	{
		from = NULL;
		initial_view = false;
	}
	else
	{
		from = create_screen_size_surface( SDL_SWSURFACE );
		SDL_BlitSurface( screen, NULL, from, NULL );
	}
	
	to = create_screen_size_surface( SDL_SWSURFACE );
	view->initialize( to );
	effect->do_switch( from, to );
	SDL_BlitSurface( to, NULL, screen, NULL );
	SDL_Flip( screen );
	
	return view->main_loop( );
}

void view_initialize( void )
{
	int i;
	
	for( i = 0; i < VIEW_MAX; i++ )
	{
		if( view_repo[i] )
		{
			if( view_repo[i]->load )
				view_repo[i]->load( );
			
			if( view_repo[i]->unload )
				atexit( view_repo[i]->unload );
		}
	}
}
