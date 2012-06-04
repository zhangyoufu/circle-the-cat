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

int dis_to_border( int row, int col, int path[][2] )
{
    int LENTH = ROWS * COLS;
    int queue[ROWS * COLS][2], l = 0, r = -1,
        tmp_dis[ROWS * COLS],
        father[ROWS][COLS][2];

    bool vis[ROWS][COLS] = {{false,},};

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
            next_row, next_col,
            tmp_row, tmp_col,
            ptr;

        for(i = 1; i < DIRECTION_MAX; i++)
        {
            if(can_move(now_row, now_col, i))
            {
                next_row = now_row;
                next_col = now_col;

                calc_cell_move(&next_row, &next_col, i);

                if(vis[next_row][next_col] == 0)
                {
                    father[next_row][next_col][0] = now_row;
                    father[next_row][next_col][1] = now_col;

                    if(on_border(next_row, next_col))
                    {
                        if(path != NULL)
                        {
                            ptr = tmp_dis[l];
                            while(ptr >= 0)
                            {
                                path[ptr][0] = next_row;
                                path[ptr][1] = next_col;

                                tmp_row = father[next_row][next_col][0];
                                tmp_col = father[next_row][next_col][1];
                                next_row = tmp_row;
                                next_col = tmp_col;

                                ptr--;
                            }
                        }
                        return tmp_dis[l] + 1;
                    }

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

    for(i = 1; i < DIRECTION_MAX; i++)
	{
		row = cat.row;
		col = cat.col;

		if(can_move(row, col, i))
		{
			calc_cell_move(&row, &col, i);
		    dis = dis_to_border(row, col, NULL);

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

Direction decision_hard(int last_row, int last_col)
{
    int i, j,
        row, col, try_row, try_col,
        dis, dire = -1,
        min_sum_dis = -1, maxdis = 100,
        tmp_dis,
        sum,
        path[ROWS * COLS][2];
    bool ok = 0;

    for(i = 1; i < DIRECTION_MAX; i++)
	{
		row = cat.row;
		col = cat.col;

		if(can_move(row, col, i))
		{
			calc_cell_move(&row, &col, i);
		    dis = dis_to_border(row, col, path);
            sum = 0;

            if(dis != -1)
            {
                ok = 1;
                for(j = 0; j < dis; j++)
                {
                    try_row = path[j][0];
                    try_col = path[j][1];

                    cell[try_row][try_col].type = CELL_BARRIER;
                    tmp_dis = dis_to_border(row, col, NULL);
                    cell[try_row][try_col].type = CELL_GROUND;

                    if(tmp_dis == -1)
                        sum += maxdis;
                    else
                        sum += tmp_dis;
                }
                if(min_sum_dis == -1 || sum < min_sum_dis)
                {
                    dire = i;
                    min_sum_dis = sum;
                }
            }
		}
	}
	if(!ok)
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

	return decision_hard(last_row, last_col);
}
