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

    tilesToUncover = vector<TileLoc>();

    numFlaggedTiles = 0;


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
        cout << "NUM OF uncovered TILES TOTAL: " << numUncoveredTiles << endl;
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
    cout << agentX << " " << agentY << "sizeof todo" << tilesToUncover.size() << "number: " << number << endl;
    //procedure: init-uncover, then getAction(result) --> location-uncover || leave
        //location-uncover 

    // int effectiveLabel = calculateEffectiveLabel(agentX, agentY, number);

    if (number == 0)
    {
        for (int dx = -1; dx <= 1; ++dx) 
        {
            for (int dy = -1; dy <= 1; ++dy) 
            {
                int nx = agentX + dx;
                int ny = agentY + dy;
                cout << nx << " vs " << colDimension << ", " << ny << " vs " << rowDimension;
		        
                if (nx >= 0 && nx < colDimension && ny >= 0 && ny < rowDimension && !(dx == 0 && dy == 0)) 
                {
                    cout << "yes" << endl;

                    if (!playerBoard[nx][ny].flag && !playerBoard[nx][ny].uncovered && !contains(tilesToUncover, TileLoc(nx, ny)))
                    {
                        cout << "UNCOVERING ALL NEIGHBORS AROUND(adding to queue) " << agentX << " " << agentY << ": " << nx << " " << ny << endl;
                        tilesToUncover.push_back( TileLoc(nx, ny));

                    }
                }
                else
                {
                    cout << "no" << endl;
                }
            }
        }

        TileLoc nextTile = tilesToUncover.back();
        cout << "revealing" << nextTile.x << " vs " << colDimension << ", " << nextTile.y << " vs " << rowDimension << endl;
        contains(tilesToUncover, TileLoc(-1, -1));

        while (playerBoard[nextTile.x][nextTile.y].uncovered) //get next covered tile (ik, )
        {
            // std::cout << "failed at " << nextTile.x << ", " << nextTile.y << "- remaining" << tilesToUncover.size() << std::endl;

            tilesToUncover.pop_back();
            TileLoc nextTile = tilesToUncover.back();
        }
        cout << "revealing" << nextTile.x << " vs " << colDimension << ", " << nextTile.y << " vs " << rowDimension << endl;

        playerBoard[nextTile.x][nextTile.y].uncovered = true;

        ++numUncoveredTiles;
        // Return the action to uncover the tile
        agentX = nextTile.x;
        agentY = nextTile.y;
        return {UNCOVER, nextTile.x, nextTile.y};
    }
    else
    {
        //assume number is 1 (we r only testing easy after all iirc?)
        //in this case we can ignore all its neighbors for now, keep taking from the queue, or, if the queue is empty 
        //(the small chance that we receive a 1 at the beginning which i think is the only case that would prevent us 
        //from getting to every tile by adjacency of 0's from agentx)
        if (tilesToUncover.empty())
        {
            for (int x = -2; x < 3; ++x)
            {
                if (x == -2 || x == 2)
                {
                    for (int y = -2; y < 3; ++y)
                    {
                        int nx = agentX + x;
                        int ny = agentY + y;

                        if (nx >= 0 && nx < colDimension && ny >= 0 && ny < rowDimension)
                        {
                            if (!playerBoard[nx][ny].uncovered && !contains(tilesToUncover, TileLoc(nx, ny)))
                            {
                                tilesToUncover.push_back( TileLoc(nx, ny));
                            }

                        }
                    }
                }
                else
                {
                    int nx = agentX + x;
                    int ny = agentY - 2;
                    int nyy = agentY + 2;
                    if (nx >= 0 && nx < colDimension && ny >= 0 && ny < rowDimension)
                    {
                        if (!playerBoard[nx][ny].uncovered && !contains(tilesToUncover, TileLoc(nx, ny)))
                        {
                            tilesToUncover.push_back( TileLoc(nx, ny));
                        }

                    }
                    if (nx >= 0 && nx < colDimension && nyy >= 0 && nyy < rowDimension)
                    {
                        if (!playerBoard[nx][nyy].uncovered && !contains(tilesToUncover, TileLoc(nx, ny)))
                        {
                            tilesToUncover.push_back( TileLoc(nx, nyy));
                        }
                    }
                }
            }

            //if still empty, uncover all except for closest tile to the center of board; (sorry its so messy lol)
            if (tilesToUncover.empty())
            {
                vector<TileLoc> lastfewcovered = vector<TileLoc>();
                for (int x = 0; x < colDimension; x++)
                {
                    for (int y = 0; y < rowDimension; y++)
                    {
                        if (!playerBoard[x][y].uncovered && !contains(tilesToUncover, TileLoc{x, y}))
                        {
                            lastfewcovered.push_back(TileLoc{x, y});
                        }
                    }
                }
                int min = INT_MAX;

                for (int i = 0; i < lastfewcovered.size(); i++)
                {
                    if (originDiff(lastfewcovered[i].x, lastfewcovered[i].y) < min)
                    {
                        min = originDiff(lastfewcovered[i].x, lastfewcovered[i].y);
                    }
                }
                for (int i = 0; i < lastfewcovered.size(); i++)
                {
                    if (originDiff(lastfewcovered[i].x, lastfewcovered[i].y) != min)
                    {
                        tilesToUncover.push_back(lastfewcovered[i]);
                    }
                }

                TileLoc nextTile = tilesToUncover.back();
                while (playerBoard[nextTile.x][nextTile.y].uncovered) 
                {
                    // std::cout << "failed at " << nextTile.x << ", " << nextTile.y << "- remaining" << tilesToUncover.size() << std::endl;

                    tilesToUncover.pop_back();
                    TileLoc nextTile = tilesToUncover.back();
                }
                playerBoard[nextTile.x][nextTile.y].uncovered = true;

                ++numUncoveredTiles;
                // Return the action to uncover the tile
                agentX = nextTile.x;
                agentY = nextTile.y;
                return {UNCOVER, nextTile.x, nextTile.y};


            }
            else
            {
                TileLoc nextTile = tilesToUncover.back();
                while (playerBoard[nextTile.x][nextTile.y].uncovered) 
                {
                    // std::cout << "failed at " << nextTile.x << ", " << nextTile.y << "- remaining" << tilesToUncover.size() << std::endl;

                    tilesToUncover.pop_back();
                    TileLoc nextTile = tilesToUncover.back();
                }
                playerBoard[nextTile.x][nextTile.y].uncovered = true;

                ++numUncoveredTiles;
                // Return the action to uncover the tile
                agentX = nextTile.x;
                agentY = nextTile.y;
                return {UNCOVER, nextTile.x, nextTile.y};

            }

        }
        else
        {
            // std::cout << "nonempty" << std::endl;
            TileLoc nextTile = tilesToUncover.back();
            while (playerBoard[nextTile.x][nextTile.y].uncovered) 
            {
                // std::cout << "failed at " << nextTile.x << ", " << nextTile.y << "- remaining" << tilesToUncover.size() << std::endl;
                tilesToUncover.pop_back();
                nextTile = tilesToUncover.back();
            }
            playerBoard[nextTile.x][nextTile.y].uncovered = true;

            ++numUncoveredTiles;
            // Return the action to uncover the tile
            agentX = nextTile.x;
            agentY = nextTile.y;
            return {UNCOVER, nextTile.x, nextTile.y};

        }
    }

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

int MyAI::originDiff(int x, int y)
{
    return (abs(rowDimension / 2 - y) + abs(colDimension / 2 - x));
}

bool MyAI::contains(vector<TileLoc> a, TileLoc b)
{

    for (int i = 0; i < a.size(); i++)
    {
        cout << " {a[" << i << "].x: " << a[i].x << " a[" << i << "].y: " << a[i].y << "}" ;

        if (a[i].x == b.x && a[i].y == b.y)
        {
            return true;
        }
        
    }
    cout << endl;
    return false;



}



// ======================================================================
// YOUR CODE ENDS
// ======================================================================
