#include "global.h"

void set_cursor( Cursor cursor )
{
	switch( cursor )
	{
		case CURSOR_NORMAL:
			SDL_ShowCursor( SDL_DISABLE );
			break;
		case CURSOR_HAND:
			SDL_ShowCursor( SDL_ENABLE );
			break;
	}
}
