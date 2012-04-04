#ifndef GLOBAL_H
#define GLOBAL_H

#pragma once

#include <stdio.h>
// #include "stdint.h"
#include "stdbool.h"

#include "SDL.h"
#include "SDL_Image.h"

#ifdef __cplusplus
extern "C" {
#endif

/*******************************************************************************
 * Macro
 ******************************************************************************/
#define UNUSED(x)			(void)(x)
 
/*******************************************************************************
 * Global parameter
 ******************************************************************************/
#define SCREEN_WIDTH		800
#define SCREEN_HEIGHT		560

#define ROWS				11
#define COLS				11

#define INITIAL_VIEW		GAME_VIEW

#define TRANSPARENT_COLOR	0x00ffffff

/*******************************************************************************
 * View and effect
 ******************************************************************************/

/* Enumeration */
enum {
	GAME_VIEW,
	TOP_SCORE_VIEW,
	VIEW_MAX,
};

enum {
	NO_EFFECT,
	FADE_EFFECT,
	/* TODO: Add card flip effect if possible */
	EFFECT_MAX,
};

/* View descriptor */
typedef uint32_t ViewDescriptor;

#define VIEW_DESCRIPTOR(v,e)	( ((e) << 16) | ((v) & 0xffff) )
#define EXTRACT_VIEW(x)			((x) & 0xffff)
#define EXTRACT_EFFECT(x)		((x) >> 16)

/* View */
typedef struct {
	void			(*load)			( void );
	void			(*initialize)	( SDL_Surface *surface );
	ViewDescriptor	(*main_loop)	( void );
	void			(*finalize)		( void );
	void			(*unload)		( void );
} View;

/* View switch effect */
typedef struct {
	void			(*do_switch)	( SDL_Surface *from, SDL_Surface *to );
} Effect;

/******************************************************************************/

/* Cell */
typedef enum {
	CELL_GROUND,
	CELL_BARRIER,
	CELL_TYPE_MAX,
} CellType;

typedef struct {
	CellType 	type;
} Cell;

/* Direction */
typedef enum {
	UNABLE_TO_MOVE,
	RIGHT_UP,
	RIGHT,
	RIGHT_DOWN,
	LEFT_DOWN,
	LEFT,
	LEFT_UP,
	DIRECTION_MAX,
} Direction;

/* Cat */
typedef struct {
	int			x, y;
	int			row, col;
	Direction	direction;
} Cat;

/******************************************************************************/

/* Global variable */
extern SDL_Surface *screen;
extern View game_view, top_score_view;
extern Effect no_effect, fade_effect;

extern Cell cell[ROWS][COLS];
extern Cat cat;

/******************************************************************************/

/* Board */
void apply_board( SDL_Surface *surface );
void board_update( int row, int col );
void draw_all( SDL_Surface *surface, int frame );
void calc_cell_position( int row, int col, int *x, int *y );
bool find_cell( int x, int y, int *row, int *col );
bool calc_cell_move( int *row, int *col, Direction dir );
void calc_position_move( int *x, int *y, Direction direction );
void board_load( void );
void board_initialize( void );
void board_finalize( void );

/* Cat */
void draw_cat( SDL_Surface *surface, int frame );
bool cat_move( void );
bool cat_in_screen( void );
void cat_load( void );
void cat_initialize( void );

/* Computer */
void computer_initialize( void );
Direction computer_decision( int last_row, int last_col );

/* View */
void view_init( void );
int show_view( ViewDescriptor desc );

/*******************************************************************************
 * Cursor
 ******************************************************************************/
typedef enum {
	CURSOR_NORMAL,
	CURSOR_HAND,
} Cursor;

void set_cursor( Cursor cursor );

/******************************************************************************/

/* Utility function */
SDL_Surface* create_surface( int width, int height, int bpp, Uint32 flags );
SDL_Surface* create_screen_size_surface( Uint32 flags );
Uint32 get_pixel( SDL_Surface *surface, int x, int y );
SDL_Surface* load_resource( const char *name );
SDL_Surface* load_resource_for_RGBA( const char *name );
void apply_background( SDL_Surface *surface );
void check_quit_event( void );
void get_input_event( SDL_Event *event );
void delay( int time );
bool calc_cell_move( int *row, int *col, Direction dir );

/******************************************************************************/

/* Score */
bool is_top_score( void );

/******************************************************************************/

#ifdef __cplusplus
}
#endif

#endif /* GLOBAL_H */
