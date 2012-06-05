#include "global.h"
#include <stdlib.h>
#include <time.h>
#include <stdio.h>

Direction (*decision_ptr) (int last_row, int last_col);

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

	UNUSED( last_row );
	UNUSED( last_col );

    possible_move_count = 0;
	for(i = 1; i < DIRECTION_MAX; i++)
	{
		row = cat.row;
		col = cat.col;

		if(can_move(row, col, i))
			possible_move[ possible_move_count++ ] = i;
	}

	if( dis_to_border(cat.row, cat.col, NULL) == -1 )
		return UNABLE_TO_MOVE;

	return possible_move[ rand( ) % possible_move_count ];
}

Direction decision_normal(int last_row, int last_col)
{
    int i, row, col, dis,
        dire = -1,
        min_dis = -1;

	UNUSED( last_row );
	UNUSED( last_col );

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
        dis, dire = -1, maxdis = 100,
        tmp_dis,
        sum, die_path,
        path[ROWS * COLS][2];
    bool ok = 0;
	double sec_path_average,
		   rank,
		   min_rank = 100000;

	UNUSED( last_row );
	UNUSED( last_col );

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
				die_path = 0;
                for(j = 0; j < dis; j++)
                {
                    try_row = path[j][0];
                    try_col = path[j][1];

					//printf("%d %d\n",try_row,try_col);

                    cell[try_row][try_col].type = CELL_BARRIER;
                    tmp_dis = dis_to_border(row, col, NULL);
                    cell[try_row][try_col].type = CELL_GROUND;

                    if(tmp_dis == -1)
                        die_path++;
                    else
                        sum += tmp_dis;
                }
				if(dis == 0)
					rank = 0;
				else
				{
				    if(dis == die_path)
				    {
				        sec_path_average = maxdis;
				    }
                    else
                    {
                        sec_path_average = (double)sum / (double)(dis-die_path);
                    }
				    if(die_path)
				    {
				        rank = 1000 + dis;
						//rank = 1000 + sec_path_average * 0.4 + dis-die_path;
				    }
                    else
                    {
						//rank = (double)(sum+dis) / (double)(dis+1);
                        rank = sec_path_average * 4 + dis * 0.5;
                    }
				}
				//printf("rank of %d, %d = %lf\n",row,col,rank);
				//printf("sec_path_average = %lf, diepath = %d, dis = %d\n",sec_path_average,die_path,dis);
                if(rank < min_rank)
                {
                    dire = i;
                    min_rank = rank;
                }
            }
			//printf("\n");
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

	return (*decision_ptr)(last_row, last_col);
}

/*
    Initialize the ground
*/
void computer_initialize(void)
{
    int i = 0,
        level;
	
	srand(time(NULL));
	level = rand() % 11 + 1;

    while(i < level)
    {
        int x = rand() % ROWS,
            y = rand() % COLS;

		if(!(x == 5 && y == 5))
		{
			cell[x][y].type = CELL_BARRIER;
			if(dis_to_border(5, 5, NULL) != -1)
			{
				board_update(x, y);
				i++;
			}
			else
				cell[x][y].type = CELL_GROUND;
		}
    }

    if(level <= 3)
        decision_ptr = decision_easy;
    else if(level <= 7)
        decision_ptr = decision_normal;
    else
        decision_ptr = decision_hard;
}
