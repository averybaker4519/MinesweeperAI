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
        cout << "NUM OF FLAGGED TILES TOTAL: " << numFlaggedTiles << endl;
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
    cout << agentX+1 << " " << agentY+1 << "number:" << number << endl;
    // Calculate effective label of the current tile
    int effectiveLabel = calculateEffectiveLabel(agentX, agentY, number);

    // Apply rules based on the effective label
    if (effectiveLabel == 0) {
        // If effective label is 0, all unmarked neighbors are safe to uncover
        for (int dx = -1; dx <= 1; ++dx) {
            for (int dy = -1; dy <= 1; ++dy) {
                int nx = agentX + dx;
                int ny = agentY + dy;
		        
                if (nx >= 0 && nx < colDimension && ny >= 0 && ny < rowDimension && !(dx == 0 && dy == 0) && !playerBoard[nx][ny].flag && !playerBoard[nx][ny].uncovered) {
                    cout << "UNCOVERING ALL NEIGHBORS AROUND " << agentX+1 << " " << agentY+1 << ": " << nx+1 << " " << ny+1 << endl;
                    // Uncover the tile
                    playerBoard[nx][ny].uncovered = true;
                    playerBoard[lastX][lastY].number = number;
                    // Update the count of uncovered tiles
                    ++numUncoveredTiles;
                    // Return the action to uncover the tile
                    lastX = nx;
                    lastY = ny;
                    return {UNCOVER, nx, ny};
                }
            }
        }
    } 
    else if (effectiveLabel == getNumUnmarkedNeighbors(agentX, agentY)) {
        cout << "Marking all neighbors around " << agentX+1 << " " << agentY+1 <<  endl;
        // If effective label equals the number of unmarked neighbors, mark all unmarked neighbors as mines
        markUnmarkedNeighbors(agentX, agentY);
        // Return LEAVE action as marking is done and no further action is required
        
    }
    
    // Update agent's position to the last uncovered tile
    agentX = lastX;
    agentY = lastY;

    // Uncover the current tile if effective label is not 0 or the number of unmarked neighbors
    cout << "Uncovering tile: " << agentX+1 << " " << agentY+1 << endl;
    if (!playerBoard[agentX][agentY].uncovered)
       ++numUncoveredTiles;
    playerBoard[agentX][agentY].uncovered = true;
    return {UNCOVER, agentX, agentY}; 
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

    //cout << "Effective label of " << x+1 << " " << y+1 << ": " << number - numMarkedNeighbors << endl;
    return playerBoard[x][y].number - numMarkedNeighbors;
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
                cout << "Unmarked: " << nx+1 << " " << ny+1 << endl;
                ++numUnmarkedNeighbors;
            }
        }
    }
    cout << "Num unmarked neighbors around " << x+1 << " " << y+1 << ": " << numUnmarkedNeighbors << endl;
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
                cout << "Marking " << nx+1 << " " << ny+1 << endl;
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
