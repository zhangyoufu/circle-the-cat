#include "global.h"
#include <stdlib.h>
#include <time.h>
#include <stdio.h>

/*
    Initialize the ground
*/
void computer_initialize(void)
{
    int i = 0;

    while(i < 10)
    {
        int x = rand() % ROWS,
            y = rand() % COLS;

		if(!(x == 5 && y == 5))
		{
			cell[x][y].type = CELL_BARRIER;
			board_update(x, y);
			i++;
		}
    }
	/*
	cell[2][3].type = CELL_BARRIER;
    board_update(2, 3);
	cell[2][5].type = CELL_BARRIER;
    board_update(2, 5);
	cell[1][4].type = CELL_BARRIER;
    board_update(1, 4);
	cell[1][3].type = CELL_BARRIER;
    board_update(1, 3);
	cell[3][3].type = CELL_BARRIER;
    board_update(3, 3);
	cell[3][4].type = CELL_BARRIER;
    board_update(3, 4);
	*/
	/*
	for(i=1;i<DIRECTION_MAX;i++)
	{
		int x=2,y=4;
		calc_cell_move(&x,&y,i);	
		printf("%d %d \n",x,y);
	}
	*/
}

/*
    Check if cat is on the border.
    Return true if on the border, false if not.
    dir is the direction which cat run off the ground. -1 if not on border.
*/
bool go_out_border(int row, int col, int *dir)
{
    if(row == 0 || row == ROWS - 1)
    {
        if(dir != NULL)
            *dir = cat.direction;
        return 1;
    }

	if(col == 0)
	{
		if(dir != NULL)
		{
            if(row & 1)
                *dir = LEFT;
            else
                *dir = cat.direction;
		}
        return 1;
	}

	if(col == COLS - 1)
	{
	    if(dir != NULL)
	    {
            if(!(row & 1))
                *dir = RIGHT;
            else
                *dir = cat.direction;
	    }
        return 1;
	}

	return 0;
}

bool on_border(int row, int col)
{
    return go_out_border(row, col, NULL);
}

bool can_move(int row, int col, int dir)
{
    return calc_cell_move( &row, &col, dir ) && cell[row][col].type == CELL_GROUND;
}

int dis_to_border(int row, int col)
{
    int LENTH = ROWS * COLS;
    int queue[ROWS * COLS][2], l = 0, r = -1,
        tmp_dis[ROWS * COLS];

    bool vis[ROWS][COLS] = {0};

	if(on_border(row, col))
        return 0;

    queue[++r][0] = row;
    queue[r][1] = col;
    tmp_dis[r] = 0;
    vis[row][col] = 1;

    while(l <= r)
    {
        int i;
        int now_row = queue[l][0],
            now_col = queue[l][1],
            next_row,
            next_col;
		/*
		printf("now: %d %d\n",now_row,now_col);
		*/
        for(i = 1; i < DIRECTION_MAX; i++)
        {
            if(can_move(now_row, now_col, i))
            {
                next_row = now_row;
                next_col = now_col;

                calc_cell_move(&next_row, &next_col, i);
				/*
				printf("can move to %d %d %d\n", next_row,next_col,i);
				*/
                if(vis[next_row][next_col] == 0)
                {
                    if(on_border(next_row, next_col))
                        return tmp_dis[l] + 1;

                    r = (r + 1) % LENTH;

                    queue[r][0] = next_row;
                    queue[r][1] = next_col;
                    vis[next_row][next_col] = 1;
                    tmp_dis[r] = tmp_dis[l] + 1;
                }
            }
        }

        l = (l + 1) % LENTH;
    }

    return -1;
}

Direction decision_easy(int last_row, int last_col)
{
    int possible_move[DIRECTION_MAX], possible_move_count;
    int i, row, col;

    possible_move_count = 0;
	for(i = 1; i < DIRECTION_MAX; i++)
	{
		row = cat.row;
		col = cat.col;

		if(can_move(row, col, i))
			possible_move[ possible_move_count++ ] = i;
	}

	if( possible_move_count == 0 )
		return UNABLE_TO_MOVE;

	return possible_move[ rand( ) % possible_move_count ];
}

Direction decision_normal(int last_row, int last_col)
{
    int i, row, col, dis,
        dire = -1,
        min_dis = -1;
	/*
	printf("now:%d %d\n",cat.row,cat.col);
	*/
    for(i = 1; i < DIRECTION_MAX; i++)
	{
		row = cat.row;
		col = cat.col;

		if(can_move(row, col, i))
		{
			calc_cell_move(&row, &col, i);

		    dis = dis_to_border(row, col);
			/*
			printf("row:%d col:%d dis:%d\n",row,col,dis);
			*/
		    if(dis != -1 && (dis < min_dis || min_dis == -1))
		    {
		        min_dis = dis;
		        dire = i;
		    }
		}
	}
	if(min_dis == -1)
        return UNABLE_TO_MOVE;
    else
        return dire;
}

Direction computer_decision(int last_row, int last_col)
{
	int dir;

	UNUSED( last_row );
	UNUSED( last_col );

    if(go_out_border(cat.row, cat.col, &dir))
        return dir;

	return decision_normal(last_row, last_col);
}
