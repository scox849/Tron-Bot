/**************************/
/* Samuel Cox             */
/* CS-241-002             */
/* AI used for tron game. */
/* Chooses the best move  */
/* based on the path with */
/* the largest number of  */
/* open spaces.           */
/**************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "tron.h"

#define CLEAR 46
#define TRAIL 16
#define OCCUPIED 35
#define TOP 0
#define BOTTOM 1
#define NO_DIR 0
#define ALL_DIR 15
#define NEAR_START_CYCLE 6
#define NONE -1


static int grid[MAX_GRID_WIDTH][MAX_GRID_HEIGHT];

static void occupyEdges(int width, int height);
static void clearGrid();
static void printGrid(int width, int height);
static void placeWalls(struct Wall *wallList);
static void placeCycles(struct Cycle *cycleList);
static void clearDeadCycles(int limit);
static int isDeadCycle(int limit, char cycle);
static int checkMove(struct Cycle *currentCycle, short jump);
static int choseNextMove(int moves, short x, short y);
static int isSafe(int posMove, short x, short y);
static int isClear(int x, int y, int xOff, int yOff);
static int pointCheck(short x, short y);
static void pickMove(int moves, struct Cycle *currentCycle, int turn);
static int canJump(short x, short y);

static char livingCycles[MAX_CYCLES];
static int position;
static int xwidth;
static int xheight;





/* Returnes the name of my player. */
char* Samuel_Cox_getName()
{
    char* name = "DerekTheCool";
    return name;
}
/*Initailizes a new game, setting up the board by placing the 
  static walls where they belong, makeing the edges of the grid 
  occupied so that cycles cannot go to them and clearing the rest
  of the spaces based on the given size of the board.*/

void Samuel_Cox_init(struct InitData *data)
{
    int gridWidth = data -> gridWidth;
    int gridHeight = data -> gridHeight;
    xwidth = gridWidth;
    xheight = gridHeight;
    if(strcmp((data -> name[0]), Samuel_Cox_getName()) == 0)
    {
        position = TOP;        
    }
    else
    {
        position = BOTTOM;
        
    }

    clearGrid(gridWidth, gridHeight);
    occupyEdges(gridWidth,gridHeight);
    placeWalls(data -> wallList);
    // printGrid(gridWidth,gridHeight);
}


/* Places the static walls on the board.*/
static void placeWalls(struct Wall *wallList)
{
    while(wallList != NULL)
    {
        grid[wallList -> x][wallList -> y] = OCCUPIED;
        wallList = wallList -> next;
    }
}

/*Prints out a text representaion of the current 
  state of the grid.                            */
static void printGrid(int width, int height)
{
    printf("\n");
    for(int i = 0; i <height; i++)
    {
        for(int j = 0; j < width; j++)
        {
            printf("%c ", grid[j][i]);
        }
        printf("\n");
    }
}

/* Checks if a given cycle is dead based on the
   current living cycles.                      */
static int isDeadCycle(int limit, char cycle)
{
    for(int i = 0; i < limit; i++)
    {
        if(livingCycles[i] == cycle)
        {
            return FALSE;
        }
    }
    return TRUE;
}

/* Removes the dead cycles from the game board. */
static void clearDeadCycles(int limit)
{
    for(int i = 1; i < (xwidth - 1); i++)
    {
        for(int j = 1; j < (xheight - 1); j++)
        {
            if(grid[i][j] != CLEAR && grid[i][j] != OCCUPIED && isDeadCycle(limit, grid[i][j]))
            {
                grid[i][j] = CLEAR;
            }
        }
    }
}

/* Places each of the current living cycles on the board
   based on the cycles is the given cycleList.          */
static void placeCycles(struct Cycle *cycleList)
{
    char nameOfCycle;
    int indexOfCycle = 0;
    while(cycleList != NULL)
    {
        nameOfCycle = cycleList -> cycleName;
        grid[cycleList -> x][cycleList -> y] = nameOfCycle;
        livingCycles[indexOfCycle] = nameOfCycle;
        indexOfCycle++;
        cycleList = cycleList -> nextCycle;     
    }
    if(indexOfCycle != (MAX_CYCLES - 1))
    {
        clearDeadCycles(indexOfCycle);    
    }
    
    
}

/*Checks the open directions based on the position 
  of the given cycle.                            */
static int checkMove(struct Cycle *currentCycle, short jump)
{
    int moves = NO_DIR;
    short x = currentCycle -> x; 
    short y = currentCycle -> y;
    int tempX;
    int tempY;
    for(int i = 0; i < 4; i++)
    { 
        tempX = x + DIR_DX[i];
        tempY = y + DIR_DY[i];  
        if(grid[tempX][tempY] == CLEAR)
        {
            moves = moves | DIR_LIST[i];
        }
        
    }
    return moves;
}

/* Checks if a space is empty based on the x,y position of 
   a cycle and an offset.                                 */
static int isClear(int x, int y, int xOff, int yOff)
{
    if(grid[x + xOff][y + yOff] != CLEAR)
    {
        return FALSE;
    }
    return TRUE;
}

/*Checks the open directions from a given x,y position.*/
static int pointCheck(short x, short y)
{
    int tempX,tempY;
    int moves = 0;

    for(int i = 0; i < 4; i++)
    { 
        tempX = x + DIR_DX[i];
        tempY = y + DIR_DY[i];  
        if(grid[tempX][tempY] == CLEAR)
        {
            moves++;
        }
        
    }
    return moves;    
}

/*Checks 101 spaces out in a given direction and makes
  that the path is clear.                          */
static int isSafe(int posMove, short x, short y)
{
    int checkedMove;
    int spacesToCheck = 101;
    int distance = 0;
    for(int i = 0; i < spacesToCheck; i++)
    {
        x += DIR_DX[posMove];
        y += DIR_DY[posMove];
        if(grid[x][y] == CLEAR)
        {
            checkedMove = pointCheck(x,y);
            if(checkedMove != 0)
            {
                distance++;
            }    
        }
        else
        {
            return distance;
        }
        
    }
    return distance;
}


/* Returns the index used in the DIR arrarys of the next move if 
   the move is a safe move.                                    */
static int choseNextMove(int moves, short x, short y)
{
    int distance = 0;
    int indexOfBestMove = NONE;
    int bestMove = NONE;
    for(int i = 0; i < 4; i++)
    {
        if(DIR_LIST[i] & moves)
        {
           if((distance = isSafe(i, x ,y)) > 0)
           {
               if(distance >= bestMove)
               {
                   bestMove = distance;
                   indexOfBestMove = i;
               }

           }
        }
    }

    return indexOfBestMove;
    
}

/*Checks if a given x, y coordinate is in bounds.
  Used for checking if a jump would work.       */
static int isInBounds(short x, short y)
{
    if(x < xwidth && x > 0 && y < xheight && y > 0)
    {
        return TRUE;
    }
    return FALSE;
}


/* Checks if a cycle can jump in any direction and picks
   the jump with the most possible moves on the other side.*/
static int canJump(short x, short y)
{
    int dirOffset = 2;
    int bestJump = NONE;
    int distance = 0;
    int tempDistance = 0;
    short tempX,tempY;
    for(int i = 0; i < 4; i++)
    {
        tempX = x + (DIR_DX[i] * dirOffset);
        tempY = y + (DIR_DY[i] * dirOffset);
        if(isInBounds(tempX,tempY))
        {
            if(grid[tempX][tempY] == CLEAR)
            {
                tempDistance = isSafe(i,tempX,tempY);
                if(tempDistance >= distance)
                {
                    distance = tempDistance;
                    bestJump = i;
                }
            
            }        
        }
        
    }
    return bestJump;    
}


/* Picks the next move that the given cycle is going to make.*/
static void pickMove(int moves, struct Cycle *currentCycle, int turn)
{
    char name = currentCycle -> cycleName;
    if((moves & ALL_DIR) == ALL_DIR)
    {
        if(position == TOP)
        {
            currentCycle -> y += DIR_DY[directionBitsToIndex(SOUTH)];     
        }
        else
        {
            currentCycle -> y += DIR_DY[directionBitsToIndex(NORTH)];  
        }
        
    }
    else
    {
        short x, y;
        x = currentCycle -> x;
        y = currentCycle -> y;
        char name  = currentCycle -> cycleName;
        int dir;
        int nextMove = NONE;

        nextMove = choseNextMove(moves, x, y);
        if(nextMove != NONE)
        {
            currentCycle -> x += DIR_DX[nextMove];
            currentCycle -> y += DIR_DY[nextMove];
            return;    
        }
        else if ((nextMove = canJump(x,y)) != NONE)
        {
            currentCycle -> x += (DIR_DX[nextMove] * 2);
            currentCycle -> y += (DIR_DY[nextMove] * 2);
            return;
            
        }

        
    }
}

/*Moves all of the cycles. */
void Samuel_Cox_move(struct MoveData *data)
{
    struct Cycle *currentCycle = data -> cycleList;
    placeCycles(currentCycle);
    int moves = NO_DIR;


    if(currentCycle -> playerIdx == position)
    {
        
        while(currentCycle -> playerIdx == position)
        {
            moves = checkMove(currentCycle, 1);
            pickMove(moves, currentCycle, data -> turnNumber);
            currentCycle = currentCycle -> nextCycle;    
        }
            
    }
    else
    {

        while(currentCycle -> playerIdx != position)
        {
            currentCycle = currentCycle -> nextCycle;
        }
        while(currentCycle != NULL)
        {
            moves = checkMove(currentCycle, 1);
            pickMove(moves, currentCycle, data -> turnNumber);
            currentCycle = currentCycle -> nextCycle;    
        }
    }
       
}

/* Initalizes the grid to clear spaces. */
static void clearGrid(int width, int height)
{
    for(int i = 1; i < (width - 1); i++)
    {
        for(int j = 1; j < (height - 1); j++)
        {
            grid[i][j] = CLEAR;
        }
    }
}

/* Sets the edges of the grid to an occupied state. */
static void occupyEdges(int width, int height)
{
    for(int i = 0; i < width; i++)
    {
        for(int j = 0; j < height; j++)
        {
            if(i == (width - 1))
            {
                grid[i][j] = OCCUPIED;
            }
            else if(j == (height - 1))
            {
                grid[i][j] = OCCUPIED;
            }
            else if(j == 0)
            {
                grid[i][j] = OCCUPIED;
            }
            else if(i == 0)
            {
                grid[i][j] = OCCUPIED;
            }
        }
    }
}


