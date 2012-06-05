#include "global.h"

/* Stolen from the mailing list */
/* Creates a new mouse cursor from an XPM */

/* XPM */
static const char *arrow[] = {
  /* width height num_colors chars_per_pixel */
  "    32    32        3            1",
  /* colors */
  "X c #000000",
  ". c #ffffff",
  "  c None",
  /* pixels */
  "X                               ",
  "XX                              ",
  "X.X                             ",
  "X..X                            ",
  "X...X                           ",
  "X....X                          ",
  "X.....X                         ",
  "X......X                        ",
  "X.......X                       ",
  "X........X                      ",
  "X.....XXXXX                     ",
  "X..X..X                         ",
  "X.X X..X                        ",
  "XX  X..X                        ",
  "X    X..X                       ",
  "     X..X                       ",
  "      X..X                      ",
  "      X..X                      ",
  "       XX                       ",
  "                                ",
  "                                ",
  "                                ",
  "                                ",
  "                                ",
  "                                ",
  "                                ",
  "                                ",
  "                                ",
  "                                ",
  "                                ",
  "                                ",
  "                                ",
  "0,0"
};

static const char *hand[] = {
  /* width height num_colors chars_per_pixel */
  "    32    32        3            1",
  /* colors */
  "X c #000000",
  ". c #ffffff",
  "  c None",
  /* pixels */
  "     XX                         ",
  "    X..X                        ",
  "    X..X                        ",
  "    X..X                        ",
  "    X..X                        ",
  "    X..XXX                      ",
  "    X..X..XXX                   ",
  "    X..X..X..XX                 ",
  "    X..X..X..X.X                ",
  "XXX X..X..X..X..X               ",
  "X..XX........X..X               ",
  "X...X...........X               ",
  " X..X...........X               ",
  "  X.X...........X               ",
  "  X.............X               ",
  "   X............X               ",
  "   X...........X                ",
  "    X..........X                ",
  "    X..........X                ",
  "     X........X                 ",
  "     X........X                 ",
  "     XXXXXXXXXX                 ",
  "                                ",
  "                                ",
  "                                ",
  "                                ",
  "                                ",
  "                                ",
  "                                ",
  "                                ",
  "                                ",
  "                                ",
  "5,0"
};

static SDL_Cursor *create_cursor( const char *image[] )
{
  int i, row, col;
  Uint8 data[4*32];
  Uint8 mask[4*32];
  int hot_x, hot_y;

  i = -1;
  for ( row=0; row<32; ++row ) {
    for ( col=0; col<32; ++col ) {
      if ( col % 8 ) {
        data[i] <<= 1;
        mask[i] <<= 1;
      } else {
        ++i;
        data[i] = mask[i] = 0;
      }
      switch (image[4+row][col]) {
        case 'X':
          data[i] |= 0x01;
          mask[i] |= 0x01;
          break;
        case '.':
          mask[i] |= 0x01;
          break;
        case ' ':
          break;
      }
    }
  }
  sscanf(image[4+row], "%d,%d", &hot_x, &hot_y);
  return SDL_CreateCursor(data, mask, 32, 32, hot_x, hot_y);
}

static SDL_Cursor *arrow_cursor, *hand_cursor;

void cursor_initialize( void )
{
	arrow_cursor = create_cursor( arrow );
	hand_cursor = create_cursor( hand );
}

void cursor_finalize( void )
{
	SDL_FreeCursor( arrow_cursor );
	SDL_FreeCursor( hand_cursor );
}

void set_cursor( Cursor cursor )
{
	switch( cursor )
	{
		case CURSOR_ARROW:
			SDL_SetCursor( arrow_cursor );
			break;
		case CURSOR_HAND:
			SDL_SetCursor( hand_cursor );
			break;
	}
}
