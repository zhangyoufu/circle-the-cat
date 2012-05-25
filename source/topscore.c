#include "global.h"
#include <math.h>

#define SCOREBOARD_WIDTH     300
#define SCOREBOARD_HEIGHT    500
#define SCOREBOARD_X_BASE    ( SCREEN_WIDTH - SCOREBOARD_WIDTH ) / 2
#define SCOREBOARD_Y_BASE    ( SCREEN_HEIGHT - SCOREBOARD_HEIGHT ) / 2
#define LETTER_WIDTH         12
#define LETTER_HEIGHT        24

static SDL_Surface *scoreboard;
static SDL_Event event;
static FILE *fp;

static SDL_Rect board_base = { SCOREBOARD_X_BASE, SCOREBOARD_Y_BASE, 0, 0 };

static int tempscore;
static int rank, stepuse, timeuse;
//static Uint32 timeuse;
static int score[10];
static char name[10][16];

static void putstring( char str[], int xx, int yy , int len )
{
	/*str as string and x,y as location and len as length*/
	int i, out_x;
	SDL_Surface *letters;
	SDL_Rect position;
	SDL_Rect letter;
	
	out_x = xx;	
	letters = load_resource_for_RGBA( "letters.gif" );
	for ( i = 0 ; i < len ; i++ )
	{
        letter.x = ( ( (Uint16) str[i] ) % 8 ) * LETTER_WIDTH;
	    letter.y = ( ( (Uint16) str[i] ) / 8 ) * LETTER_HEIGHT;
	    letter.w = LETTER_WIDTH;
	    letter.h = LETTER_HEIGHT;
		position.x = out_x;
		position.y = yy;
		SDL_BlitSurface( letters,
			             &letter,
			             scoreboard,
			             &position );
        out_x += LETTER_WIDTH;
	}
	SDL_FreeSurface( letters );
}

static void create_savedata( void )
{
	int i, j;
	char newname[10][17] = { "kuroneko\0       ",
                             "Albert\0         ",
                             "MYM\0            ",
                             "WORLDELITE\0     ",
                             "wwwwwwwwwwwwwww\0",
                             "Zhang Youfu\0    ",
                             "asdfghjkl\0      ",
                             "1+1\0            ",
                             "Q && M\0         ",
                             "DotA\0           " };
    int newscore[10] = { 80, 77, 75, 70, 66, 60, 55, 44, 30, 0 };
	fp = fopen( "savedata.sav", "wb+" );
	for( i = 0; i < 10; i++ )
	{
		for( j = 0; j < 16; j++ )
		    putc( newname[i][j], fp );
        fprintf( fp, "%d\n", newscore[i] );
	}
	fclose( fp );
}

static void open_savedata( void )
{
	fp = fopen( "savedata.sav", "rb" );
	if ( fp == NULL ){
		create_savedata( );
		fp = fopen( "savedata.sav", "rb" );
	}
}

bool is_top_score( Uint32 time, int steps )
{
    int i,j;
    bool result = false;
	
	open_savedata( );
    stepuse = steps;
    timeuse = time / 1000;
    /* judge if is topscore */
    tempscore = 120 - (int)( stepuse / 5.0 + 5 * log( timeuse ) + 2 * pow( 1.5, timeuse / 10 ) );//calculate the score
    for( i = 0; i < 10; i++ )
    {
    	for( j = 0; j < 16; j++ )
		    name[i][j] = fgetc( fp );
	    fscanf( fp, "%d\n", &score[i] );
    }
    for( i = 0; i < 10; i++ )
    {
    	if ( score[i] < tempscore )
    	{
	    	rank = i;
	    	result = true;
	    	break;
	    }
    }
    fclose( fp );	
	return result;
}

static void scorewall_load( void )
{
    SDL_Surface* scorewall;
	scorewall = load_resource_for_RGBA( "scorewall.gif" );
	SDL_BlitSurface( scorewall,
			         NULL,
			         scoreboard,
			         NULL );
	SDL_FreeSurface( scorewall );
}

static void button_load( int state )
{
	SDL_Rect position = { 100, 420, 0, 0 };
	SDL_Surface* button;
	if ( state == 0 ) button = load_resource_for_RGBA( "button0.gif" );
	if ( state == 1 ) button = load_resource_for_RGBA( "button1.gif" );
	SDL_BlitSurface( button,
			         NULL,
			         scoreboard,
			         &position );
    SDL_FreeSurface( button );
}

static void inputbox_init( void )
{
	SDL_Rect position = { 50, 360, 0, 0 };
	SDL_Surface* inputbox;
	inputbox = load_resource_for_RGBA( "inputbox.png" );
    SDL_BlitSurface( inputbox,
			         NULL,
			         scoreboard,
			         &position );
    SDL_FreeSurface( inputbox );
}

static void score_load( void )
{
	int i;
	char num[2], scorestr[3];
	/*hiscore output*/
	for( i = 0; i < 10; i++ ){
		num[1] = '1' + i;
		if ( num[1] > '9' ){
			num[1] = num[1] - 10;
			num[0] = '1';
		} 
		else num[0] = '0';
		putstring( num, 10, 90 + 24 * i, 2 );//rank output		
        putstring( name[i], 55, 90 + 24 * i, 16 );//name output        
        scorestr[0] = '0' + score[i] / 100;
        if ( scorestr[0] == '0' ) scorestr[0] = '\0';
        scorestr[1] = '0' + (score[i] / 10) % 10;
        if ( ( scorestr[1] == '0' ) && ( scorestr[0] == '\0' ) ) scorestr[1] = '\0';
        scorestr[2] = '0' + score[i] % 10;
        putstring( scorestr, 250, 90 + 24 * i, 3 );//score output
	}
}

static void tempscore_load( void )
{
	char stri[32], strj[32];
	int i, j, tscore;
	const int loc_y = 60;
	tscore = tempscore;
	/*tempscore output*/
	i = 0;
	while( tscore > 0 )
	{
		stri[i] = '0' + tscore % 10;
		tscore = tscore / 10;
		i++;
	}
	for( j = 0; j < i; j++ )
		strj[j] = stri[i-1-j];
	putstring( strj, 246, loc_y, i );
	/*steps output*/
	i = 0;
	while( stepuse > 0 )
	{
		stri[i] = '0' + stepuse % 10;
		stepuse = stepuse / 10;
		i++;
	}
	for( j = 0; j < i; j++ )
		strj[j] = stri[i-1-j];
	putstring( strj, 162, loc_y, i );
	/*time output*/
	i = 0;
	while( timeuse > 0 )
	{
		stri[i] = '0' + timeuse % 10;
		timeuse = timeuse / 10;
		i++;
	}
	for( j = 0; j < i; j++ )
		strj[j] = stri[i-1-j];
	putstring( strj, 78, loc_y, i );
}

static void save_topscore( char str[] )
{
    int i,j;

    fp = fopen( "savedata.sav", "wb" );
    i = 0;
    for( i = 0; i < rank; i++ )
    {
		for( j = 0; j < 16; j++ )
		    fputc( name[i][j], fp );
        fprintf( fp, "%d\n", score[i] );
    }
    for( j = 0; j < 16; j++ )
        fputc( str[j], fp );
    fprintf( fp, "%d\n", tempscore );
    for( i = rank; i < 9; i++ )
    {
    	for( j = 0; j < 16; j++ )
		    fputc( name[i][j], fp );
        fprintf( fp, "%d\n", score[i] );
    }
    fclose( fp );
}

static void draw_scoreboard( SDL_Surface *surface )
{
	SDL_BlitSurface( scoreboard, NULL, surface, &board_base );
	if( surface == screen )
		SDL_Flip( screen );
}

static void load( void )
{
}

static void unload( void )
{
}

static void scoreboard_initialize( void )
{
	scorewall_load( );
	score_load( );
	tempscore_load( );
	inputbox_init( );
}

static void initialize( SDL_Surface *surface )
{
	Uint32 bgcolor = SDL_MapRGB( surface->format, 0xff, 0xff, 0xff );
	SDL_FillRect( surface, NULL, bgcolor );
	scoreboard = create_surface( SCOREBOARD_WIDTH, SCOREBOARD_HEIGHT, 32, SDL_HWSURFACE );
	SDL_FillRect( scoreboard, NULL, TRANSPARENT_COLOR );
	scoreboard_initialize( );
	button_load( 0 );
	draw_scoreboard( surface );
}

static void finalize( void )
{
	SDL_FreeSurface( scoreboard );
}

static ViewDescriptor main_loop( void )
{
	char strin[16]; //string input
	char chin; //char input
	Uint16 len, alen; //string length
	Uint16 code; //key's unicode
	Uint16 cursor_x, cursor_y; //cursor location x,y
	bool cursor_over = false; //if cursor is over the button
	bool entered = false; //if pressed RETURN or click OK
	bool quit = false;
	len = 0;
	/*deal with keyboard and mouse*/
	SDL_EnableUNICODE( SDL_ENABLE );
	set_cursor( CURSOR_NORMAL );
    putstring( "_", 54, 364, 1 );
    draw_scoreboard( screen );
	while( !quit )
	{
		get_input_event( &event );
		switch( event.type )
	    {
			case SDL_KEYDOWN:
	        {
                if( entered ) break;
				if( event.key.keysym.sym == SDLK_RETURN )
                {
            	    entered = true;
            	    quit = true;
                }    
                else if( event.key.keysym.sym == SDLK_BACKSPACE )
                {
					if ( len != 0 ) len--;
                }
                else if( len < 15 )
                {
                	code = event.key.keysym.unicode;
                	if( ( code >= 32 ) && ( code <= 126 ) )
                	{
	                	chin = (char) code;
    	                strin[len] = chin;
        	            len++;
	                }
                }
                strin[len] = '_';
                inputbox_init( );
                draw_scoreboard( screen );
                if ( !entered )
		        {
			        if( len < 15 ) alen = len +1;
			        else alen = len;
					putstring( strin, 54, 364, alen );
		        }
		        draw_scoreboard( screen );
		        break;
    	    }
        	case SDL_MOUSEMOTION:
	    	{
		    	cursor_x = event.motion.x;
                cursor_y = event.motion.y;
                if ( ( cursor_x >= 100 + SCOREBOARD_X_BASE ) && ( cursor_x <= 200 + SCOREBOARD_X_BASE ) && ( cursor_y >= 420 + SCOREBOARD_Y_BASE ) && ( cursor_y <= 470 + SCOREBOARD_Y_BASE ) )
                {
					cursor_over = true;
					set_cursor( CURSOR_HAND );
                }
		    	else
				{
				    cursor_over = false;
				    set_cursor( CURSOR_NORMAL );
				}
				break;
    		}
	    	case SDL_MOUSEBUTTONDOWN:
            {
        	    if( event.button.button != SDL_BUTTON_LEFT ) break;
				if ( cursor_over )
    			{
                    scoreboard_initialize( );
					button_load( 1 );
					putstring( strin, 54, 364, len );
					draw_scoreboard( screen );
			        entered = true;
    			}
    			break;
            }
            case SDL_MOUSEBUTTONUP :
			{
			    if ( entered )
				{
				    scoreboard_initialize( );
					button_load( 0 );
					draw_scoreboard( screen );
					quit = true;
				}
				break;
			}
			if( event.type == SDL_QUIT )
			{
				entered=true;
				quit = true;
				break;
			}
    	}
		if( entered )
		{
		    strin[len] = '\0';
		    for( alen = len + 1; alen < 16; alen++ )
		        strin[alen] = ' ';
		}
		//draw_scoreboard( screen );
	}
    SDL_EnableUNICODE( SDL_DISABLE );
    save_topscore( strin );
	delay(1000);//waiting
	return VIEW_DESCRIPTOR( GAME_VIEW, NO_EFFECT );
}

View top_score_view = { load, initialize, main_loop, finalize, unload };
