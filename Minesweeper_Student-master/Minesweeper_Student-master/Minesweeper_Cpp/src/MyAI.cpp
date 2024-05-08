// ======================================================================
// FILE:        MyAI.cpp
//
// AUTHOR:      Jian Li
//
// DESCRIPTION: This file contains your agent class, which you will
//              implement. You are responsible for implementing the
//              'getAction' function and any helper methods you feel you
//              need.
//
// NOTES:       - If you are having trouble understanding how the shell
//                works, look at the other parts of the code, as well as
//                the documentation.
//
//              - You are only allowed to make changes to this portion of
//                the code. Any changes to other portions of the code will
//                be lost when the tournament runs your code.
// ======================================================================

#include "MyAI.hpp"

MyAI::MyAI ( int _rowDimension, int _colDimension, int _totalMines, int _agentX, int _agentY ) : Agent()
{
    // ======================================================================
    // YOUR CODE BEGINS
    // ======================================================================

    // initializing all variables inherited from Agent.hpp
    rowDimension = _rowDimension;
    colDimension = _colDimension;
    totalMines = _totalMines;
    agentX = _agentX;
    agentY = _agentY;


    // created a blank copy of the board to update as game progresses
    playerBoard.resize(colDimension, vector<Tile>(rowDimension));
    playerBoard[_agentX][_agentY].uncovered = true;

    numUncoveredTiles = 1;

    // ======================================================================
    // YOUR CODE ENDS
    // ======================================================================
};

Agent::Action MyAI::getAction( int number )
{
    // ======================================================================
    // YOUR CODE BEGINS
    // ======================================================================    

    if (colDimension * rowDimension - numUncoveredTiles <= totalMines)
    {
        return {LEAVE,-1,-1};
    }

    else
    {
       return ruleOfThumb(number); 
    }
        
    // ======================================================================
    // YOUR CODE ENDS
    // ======================================================================

}


// ======================================================================
// YOUR CODE BEGINS
// ======================================================================

Agent::Action MyAI::ruleOfThumb(int number)
{
    // Initialize an action to return
    Agent::Action action = {LEAVE, -1, -1};

    // Iterate over all uncovered tiles on the board
    for (int x = 0; x < colDimension; ++x) 
    {
        for (int y = 0; y < rowDimension; ++y) 
        {
            if (!playerBoard[x][y].uncovered) 
            {
                // Get the effective label of the current tile
                int effectiveLabel = calculateEffectiveLabel(x, y, number);
                //cout << x << ", " << y << ": " << effectiveLabel << endl;

                // Get the number of unmarked neighbors
                int numUnmarked = getNumUnmarkedNeighbors(x, y);

                // Apply rules of thumb
                if (effectiveLabel == numUnmarked) 
                {
                    // All unmarked neighbors must be mines
                    markUnmarkedNeighbors(x, y);
                }
                else if (effectiveLabel == 0) 
                {
                    // All unmarked neighbors must be safe to uncover
                    // Uncover them
                    playerBoard[x][y].uncovered = true;
                    ++numUncoveredTiles;
                    action = {UNCOVER, x, y}; // Update the action to uncover this tile
                    return action; // Return the action immediately after uncovering one tile
                }
            }
        }
    }

    return action; // Return the action (default is LEAVE) if no tile is uncovered}
}

// Calculate the effective label of a tile by subtracting the number of marked neighbors from its label
int MyAI::calculateEffectiveLabel(int x, int y, int number) 
{
    int numMarkedNeighbors = 0;
    for (int dx = -1; dx <= 1; ++dx) 
    {
        for (int dy = -1; dy <= 1; ++dy) 
        {
            int nx = x + dx;
            int ny = y + dy;
            if (nx >= 0 && nx < colDimension && ny >= 0 && ny < rowDimension && playerBoard[nx][ny].flag) 
            {
                ++numMarkedNeighbors;
            }
        }
    }

    return number - numMarkedNeighbors;
}

// Count the number of unmarked neighbors of a tile
int MyAI::getNumUnmarkedNeighbors(int x, int y) 
{
    int numUnmarkedNeighbors = 0;
    for (int dx = -1; dx <= 1; ++dx)
    {
        for (int dy = -1; dy <= 1; ++dy) 
        {
            int nx = x + dx;
            int ny = y + dy;
            if (nx >= 0 && nx < colDimension && ny >= 0 && ny < rowDimension && !(dx == 0 && dy == 0) && !playerBoard[nx][ny].flag && !playerBoard[nx][ny].uncovered) 
            {
                ++numUnmarkedNeighbors;
            }
        }
    }
    return numUnmarkedNeighbors;
}

// Mark all unmarked neighbors of a tile as mines
void MyAI::markUnmarkedNeighbors(int x, int y) 
{
    for (int dx = -1; dx <= 1; ++dx) 
    {
        for (int dy = -1; dy <= 1; ++dy) 
        {
            int nx = x + dx;
            int ny = y + dy;
            
            if (nx >= 0 && nx < colDimension && ny >= 0 && ny < rowDimension && !(dx == 0 && dy == 0) && !playerBoard[nx][ny].flag && !playerBoard[nx][ny].uncovered) 
            {
                playerBoard[nx][ny].flag = true;
                // Update the number of flagged tiles
                ++numFlaggedTiles;
            }
        }
    }
}

// ======================================================================
// YOUR CODE ENDS
// ======================================================================
