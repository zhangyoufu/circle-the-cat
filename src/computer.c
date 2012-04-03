#include "global.h"
#include <stdlib.h>
#include <time.h>

void computer_initialize( void )
{
}

Direction computer_decision( int last_row, int last_col )
{
	int possible_move[DIRECTION_MAX], possible_move_count;
	int i, row, col;
	
	UNUSED( last_row );
	UNUSED( last_col );
	
	if( cat.row == 0 || cat.row == ROWS )
		return cat.direction;
	
	if( cat.col == 0 )
	{
		if( cat.row & 1 )
			return LEFT;
		else
			return cat.direction;
	}
	
	if( cat.col == COLS-1 )
	{
		if( !(cat.row & 1) )
			return RIGHT;
		else
			return cat.direction;
	}
	
	possible_move_count = 0;
	for( i = 1; i < DIRECTION_MAX; i++ ) {
		row = cat.row; col = cat.col;
		if( calc_cell_move( &row, &col, i )
		    && cell[row][col].type == CELL_GROUND )
			possible_move[ possible_move_count++ ] = i;
	}
	
	if( possible_move_count == 0 )
		return UNABLE_TO_MOVE;
	
	return possible_move[ rand( ) % possible_move_count ];
}
