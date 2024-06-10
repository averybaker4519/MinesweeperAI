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
#include <list>
#include <random>



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

    tilesToUncover = vector<Tile>();
    dangerFrontier = vector<Tile>();
    // , int, MyAI::classComp>();

    numFlaggedTiles = 0;


    // created a blank copy of the board to update as game progresses
    playerBoard.resize(colDimension, vector<Tile>(rowDimension));

    for (int x = 0; x < colDimension; ++x)
    {
        for (int y = 0; y < rowDimension; ++y)
        {
            playerBoard[x][y].x = x;
            playerBoard[x][y].y = y;

            if (x == 0 || x == colDimension - 1)
            {
                if (y == 0 || y == rowDimension - 1)
                {
                    playerBoard[x][y].numCoveredNeighbors = 3; 
                }
                else
                {
                    playerBoard[x][y].numCoveredNeighbors = 5;
                }
            }

            if (y == 0 || y == rowDimension - 1)
            {
                if (x == 0 || x == colDimension - 1)
                {
                    playerBoard[x][y].numCoveredNeighbors = 3;
                }
                else
                {
                    playerBoard[x][y].numCoveredNeighbors = 5;
                }
            }


            //cerr << x << " " << y << ": " << playerBoard[x][y].numCoveredNeighbors << endl;
        }
    }


    playerBoard[_agentX][_agentY].uncovered = true;
    uncoveredFrontier.insert(playerBoard[_agentX][_agentY]);
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
        //cout <<"WEEEOOO:" << numUncoveredTiles << endl;
        return {LEAVE,-1,-1};
    }

    else
    {
        try
        {
            playerBoard[agentX][agentY].effectiveLabel = number;
            // Update covered and uncovered frontiers
            updateFrontiers();

            //cerr << tilesToUncover.size() << endl;
            // for (int i = 0; i < tilesToUncover.size(); i++)
            // {
            //     // cerr << "@" << i << " (" << tilesToUncover[i].x+1 << ", " << tilesToUncover[i].y+1 << "): " << tilesToUncover[i].effectiveLabel << " vs " << tilesToUncover[i].effectiveModifier << endl;
            // }
            if (number == 0)
            {
                return ruleOfThumb(number); 

            }
            else
            {
                return BasicHeuristic(number);
            }

        }
        catch (exception E)
        {
            //cout << E.what() << endl;
            return {LEAVE, -1, -1};
        }
        //// cerr << "NUM OF uncovered TILES TOTAL: " << numUncoveredTiles << endl;
    }
        
    // ======================================================================
    // YOUR CODE ENDS
    // ======================================================================

}

int random(int rStart, int rEnd) {
    std::random_device d;
    std::mt19937 generator(d());
    std::uniform_int_distribution<int>distr(rStart, rEnd);
    return distr(generator);
}

// ======================================================================
// YOUR CODE BEGINS
// ======================================================================

Agent::Action MyAI::ruleOfThumb(int number)
{
    //cerr << agentX << " " << agentY << "sizeof todo" << tilesToUncover.size() << "number: " << number << endl;
    //procedure: init-uncover, then getAction(result) --> location-uncover || leave
        //location-uncover 

    // int effectiveLabel = calculateEffectiveLabel(agentX, agentY, number);

    if (playerBoard[agentX][agentY].effectiveLabel + playerBoard[agentX][agentY].effectiveModifier == 0 || playerBoard[agentX][agentY].effectiveLabel == 0)
    {
        for (int dx = -1; dx <= 1; ++dx) 
        {
            for (int dy = -1; dy <= 1; ++dy) 
            {
                int nx = agentX + dx;
                int ny = agentY + dy;
                //cerr << nx << " vs " << colDimension << ", " << ny << " vs " << rowDimension;
		        
                if (nx >= 0 && nx < colDimension && ny >= 0 && ny < rowDimension && !(dx == 0 && dy == 0)) 
                {
                    //// cerr << "yes" << endl;
                    playerBoard[nx][ny].numCoveredNeighbors -= 1;
                    dangerFrontier.insert(dangerFrontier.end(), playerBoard[nx][ny]);


                    if (!playerBoard[nx][ny].flag && !playerBoard[nx][ny].uncovered && !contains(tilesToUncover, playerBoard[nx][ny])) //tileOriginDiff(playerBoard[nx][ny]) == playerBoard[nx][ny].numCoveredNeighbors+1
                    {

                        // cerr << "UNCOVERING ALL NEIGHBORS AROUND(adding to queue) " << ": " << nx+1 << ", " << ny+1 << "" << endl;
                        tilesToUncover.push_back( playerBoard[nx][ny]);
                        dangerFrontier.insert(dangerFrontier.end(), playerBoard[nx][ny]);



                        //removed subtracting effective label here
                    }
                }
            }
        }
        
        // TODO: guessing logic
        if (tilesToUncover.empty())
        {
            //TODO placeholder
            Tile next  = guess();

            if (!playerBoard[next.x][next.y].uncovered)
            {
                playerBoard[next.x][next.y].uncovered = true;

                ++numUncoveredTiles;
                // Return the action to uncover the tile
                agentX = next.x;
                agentY = next.y;
                //cout << "YAY" << endl;
                return {UNCOVER, next.x, next.y};

            }
            else
            {
                //cout << "what?" << endl;
                return {LEAVE, -1, -1};
            }

            // return {LEAVE, -1, -1};
            
        }
        else
        {
            Tile nextTile = tilesToUncover.back();            
            tilesToUncover.pop_back();

            //// cerr << "revealing" << nextTile.x << " vs " << colDimension << ", " << nextTile.y << " vs " << rowDimension << endl;
            contains(tilesToUncover, Tile(-1, -1));

            while (!tilesToUncover.empty()) //get next covered tile (ik, )
            {
                if (!playerBoard[nextTile.x][nextTile.y].uncovered)
                {
                    break;
                }
                // std::// cerr << "failed at " << nextTile.x << ", " << nextTile.y << "- remaining" << tilesToUncover.size() << std::endl;

                tilesToUncover.pop_back();
                Tile nextTile = tilesToUncover.back();
            }
            if (!playerBoard[nextTile.x][nextTile.y].uncovered)
            {
                // cerr << "revealing" << nextTile.x << " vs " << colDimension << ", " << nextTile.y << " vs " << rowDimension << "which is" << (playerBoard[nextTile.x][nextTile.y].uncovered ? "uncovered" : "not uncovereed") << endl;

                
                dangerFrontier.insert(dangerFrontier.end(), playerBoard[nextTile.x][nextTile.y]);
                
                ++numUncoveredTiles;
                // Return the action to uncover the tile
                agentX = nextTile.x;
                agentY = nextTile.y;
                playerBoard[nextTile.x][nextTile.y].uncovered = true;
                //cout << "YAY2" << endl;
                return {UNCOVER, nextTile.x, nextTile.y};
            }
        }

    }
    else
    {
        Tile next = guess();

        if (!playerBoard[next.x][next.y].uncovered)
        {
            playerBoard[next.x][next.y].uncovered = true;

            ++numUncoveredTiles;
            // Return the action to uncover the tile
            agentX = next.x;
            agentY = next.y;
            //cout << "YAY3" << endl;
            return {UNCOVER, next.x, next.y};

        }
        else
        {
            //cout << "what?" << endl;
            return {LEAVE, -1, -1};
        }



    }
    return BasicHeuristic(number);
}

Agent::Action MyAI::BasicHeuristic(int number)
{

    // decrement neighbors..
    for (int dx = -1; dx <= 1; ++dx) 
    {
        for (int dy = -1; dy <= 1; ++dy) 
        {
            int nx =agentX + dx;
            int ny = agentY+ dy;
            
            if (nx >= 0 && nx < colDimension && ny >= 0 && ny < rowDimension && !(dx == 0 && dy == 0) && !playerBoard[nx][ny].flag && !playerBoard[nx][ny].uncovered) 
            {  
                playerBoard[nx][ny].numCoveredNeighbors -= 1;

                // if (dangerFrontier.find(playerBoard[nx][ny]) == dangerFrontier.end())//
                {
                    dangerFrontier.insert(dangerFrontier.end(), playerBoard[nx][ny]);
                }
            }
        }
    }

    

    //TODO: if tilesToUncover empty, else

    if (!tilesToUncover.empty())
    {
        Tile nextTile = tilesToUncover.back();            
        tilesToUncover.pop_back();

        // contains(tilesToUncover, Tile(-1, -1));

        while (!tilesToUncover.empty()) //get next covered tile (ik, )
        {
            if (!playerBoard[nextTile.x][nextTile.y].uncovered)
            {
                break;
            }
            // std::// cerr << "failed at " << nextTile.x << ", " << nextTile.y << "- remaining" << tilesToUncover.size() << std::endl;

            tilesToUncover.pop_back();
            Tile nextTile = tilesToUncover.back();
        }
        if (!playerBoard[nextTile.x][nextTile.y].uncovered)
        {
            // cerr << "revealing" << nextTile.x << " vs " << colDimension << ", " << nextTile.y << " vs " << rowDimension << "which is" << (playerBoard[nextTile.x][nextTile.y].uncovered ? "uncovered" : "not uncovereed") << endl;

            {
                dangerFrontier.insert(dangerFrontier.end(), playerBoard[nextTile.x][nextTile.y]);
            }

            ++numUncoveredTiles;
            // Return the action to uncover the tile
            agentX = nextTile.x;
            agentY = nextTile.y;
            playerBoard[nextTile.x][nextTile.y].uncovered = true;
            //cout << "YAY4" << endl;
            return {UNCOVER, nextTile.x, nextTile.y};

        }
        //// cerr << "revealing" << nextTile.x << " vs " << colDimension << ", " << nextTile.y << " vs " << rowDimension << endl;
        // if (dangerFrontier.find(playerBoard[nextTile.x][nextTile.y]) == dangerFrontier.end())


    }


    if (dangerFrontier.size() > 2500)
    {
        vector<Tile> dangerFrontier = vector<Tile>();

    }
    else
    {
        vector<Tile> toAdd = vector<Tile>();


        for (auto iter = dangerFrontier.begin(); iter != dangerFrontier.end(); ++iter)
        {
            // cerr <<  dangerFrontier.size() <<"DANGER-(" << iter->x+1 << ", " << iter->y+1 << "): " << iter->effectiveLabel << " vs " << iter->effectiveModifier << endl;

            int lX = agentX;
            int lY = agentY;
            agentX = iter->x;
            agentY = iter->y;
            // cerr << playerBoard[agentX][agentY].effectiveLabel << " + " <<  playerBoard[agentX][agentY].effectiveModifier << " vs " << getNumCoveredNeighbors(agentX, agentY) << endl;
            // cerr << "DANGER-(" << iter->x+1 << ", " << iter->y+1 << "): is uncovered? " << playerBoard[agentX][agentY].uncovered;
            if (agentX >= colDimension || agentY >= rowDimension )
            {
                //cout << "uhoh" << endl;
                break;
            }
            if (playerBoard[agentX][agentY].effectiveLabel + playerBoard[agentX][agentY].effectiveModifier + MyAI::getNumMarkedNeighbors(agentX, agentY) == getNumCoveredNeighbors(agentX, agentY))
            {

                vector<Tile> newlyMarkedNeighbors = markUnmarkedNeighbors(agentX, agentY);

                for (auto const& element: newlyMarkedNeighbors)
                {
                    // subtracting 1 from every neighbor's effectiveModifier
                    for (int dx = -1; dx <= 1; ++dx)
                    {
                        for (int dy = -1; dy <= 1; ++dy) 
                        {
                            int nx = element.x + dx;
                            int ny = element.y + dy;
                            if (nx >= 0 && nx < colDimension && ny >= 0 && ny < rowDimension && !(dx == 0 && dy == 0) && !playerBoard[nx][ny].flag && playerBoard[nx][ny].uncovered) 
                            {
                                playerBoard[nx][ny].effectiveModifier--;
                                // cerr << "x: " << nx << ", y:" << ny << "into" << playerBoard[nx][ny].effectiveLabel << "+"  << playerBoard[nx][ny].effectiveModifier << ", then";

                                if (playerBoard[nx][ny].effectiveLabel + playerBoard[nx][ny].effectiveModifier == 0)
                                {
                                    for (int dxx = -1; dxx <= 1; ++dxx)
                                    {
                                        for (int dyx = -1; dyx <= 1; ++dyx) 
                                        {
                                            int nxx = nx + dxx;
                                            int nyx = ny + dyx;
                                            if (nxx >= 0 && nxx < colDimension && nyx >= 0 && nyx < rowDimension && !(dxx == 0 && dyx == 0) && !playerBoard[nxx][nyx].uncovered && !playerBoard[nxx][nyx].flag) 
                                            {
                                                //// cerr << "Unmarked: " << nx+1 << " " << ny+1 << endl;
                                                tilesToUncover.push_back(playerBoard[nxx][nyx]);
                                                toAdd.insert(toAdd.begin(), playerBoard[nxx][nyx]);

                                                // dangerFrontier[playerBoard[nxx][nyx]] = playerBoard[nx][ny].numCoveredNeighbors;

                                            }
                                        }
                                    }
                                }

                                //TODO : remove from dangerFrontier, if it was previously in

                            }
                            else if (nx >= 0 && nx < colDimension && ny >= 0 && ny < rowDimension && !(dx == 0 && dy == 0) && !playerBoard[nx][ny].flag && !playerBoard[nx][ny].uncovered)
                            {
                                playerBoard[nx][ny].effectiveModifier--;

                            }
                            // cerr <<endl;
                        }
                    }
                }
                // remove(dangerFrontier.begin(), dangerFrontier.end(), *iter);

                // frontier stuff
            }
            else if (playerBoard[agentX][agentY].effectiveLabel + playerBoard[agentX][agentY].effectiveModifier == 0 || playerBoard[agentX][agentY].effectiveLabel + playerBoard[agentX][agentY].effectiveModifier < 0 && playerBoard[agentX][agentY].effectiveLabel != -1  && playerBoard[agentX][agentY].uncovered && playerBoard[agentX][agentY].numCoveredNeighbors > playerBoard[agentX][agentY].effectiveLabel)
            {
                //uncover all unflagged neighbors
                for (int dx = -1; dx <= 1; ++dx) 
                {
                    for (int dy = -1; dy <= 1; ++dy) 
                    {
                        int nx = agentX + dx;
                        int ny = agentY + dy;
                        
                        if (nx >= 0 && nx < colDimension && ny >= 0 && ny < rowDimension && !(dx == 0 && dy == 0) && !playerBoard[nx][ny].flag && !playerBoard[nx][ny].uncovered) 
                        {  
                            // cerr << "eating " << nx+1 << " " << ny+1 << endl;

                            tilesToUncover.push_back(playerBoard[nx][ny]);
                        }
                    }
                }

            }
            agentX = lX;
            agentY = lY;
        }
        for (int i = 0; i < toAdd.size(); i++)
        {
            dangerFrontier.insert(dangerFrontier.end(), toAdd.at(i));

        }

    }
    

    if (!tilesToUncover.empty())
    {
        Tile nextTile = tilesToUncover.back();            
        tilesToUncover.pop_back();

        //// cerr << "revealing" << nextTile.x << " vs " << colDimension << ", " << nextTile.y << " vs " << rowDimension << endl;
        // contains(tilesToUncover, Tile(-1, -1));

        while (!tilesToUncover.empty()) //get next covered tile (ik, )
        {
            if (!playerBoard[nextTile.x][nextTile.y].uncovered)
            {
                break;
            }
            // std::// cerr << "failed at " << nextTile.x << ", " << nextTile.y << "- remaining" << tilesToUncover.size() << std::endl;

            tilesToUncover.pop_back();
            Tile nextTile = tilesToUncover.back();
        }
        if (!playerBoard[nextTile.x][nextTile.y].uncovered)
        {
            // cerr << "revealing" << nextTile.x << " vs " << colDimension << ", " << nextTile.y << " vs " << rowDimension << "which is" << (playerBoard[nextTile.x][nextTile.y].uncovered ? "uncovered" : "not uncovereed") << endl;

            {
                dangerFrontier.insert(dangerFrontier.end(), playerBoard[nextTile.x][nextTile.y]);
            }

            ++numUncoveredTiles;
            // Return the action to uncover the tile
            agentX = nextTile.x;
            agentY = nextTile.y;
            playerBoard[nextTile.x][nextTile.y].uncovered = true;
            //cout << "YAY5" << endl;
            return {UNCOVER, nextTile.x, nextTile.y};


        }
    }

    //cerr << "Made it into BasicHeuristic" << endl;  
    Tile next = guess();
    //cerr << "Made it into BasicHeuristic" << endl;

    if (!playerBoard[next.x][next.y].uncovered)
    {
        //cerr << "Made it into BasicHeuristic" << endl;
        playerBoard[next.x][next.y].uncovered = true;

        ++numUncoveredTiles;
        // Return the action to uncover the tile
        agentX = next.x;
        agentY = next.y;
        //cout << "YAY6" << endl;
        return {UNCOVER, next.x, next.y};

    }
    else
    {
        //cout << "what?" << endl;
        return {LEAVE, -1, -1};
    }


}

MyAI::Tile MyAI::guess()
{
    int k = -1;
    int l = -1;
    multimap<float, Tile> remnants = multimap<float, Tile>();

    set<Tile>::iterator itr;
    for (itr = coveredFrontier.begin(); itr != coveredFrontier.end(); itr++) 
    {
        //cout << "IN FOR LOOP: " << itr->x << " " << itr->y << endl;
        if (getNumCoveredNeighbors(itr->x, itr->y) == tileOriginDiff(playerBoard[itr->x][itr->y]) && !playerBoard[itr->x][itr->y].uncovered)
        {
            k = itr->x;
            l = itr->y;
            //cout << "K and L set" << endl;
            break;
        }
    }

    //cout << "OUT OF LOOP" << endl;

    if (k == -1)
    {
        //cout << " FAIL0";

        for (itr = coveredFrontier.begin(); itr != coveredFrontier.end(); itr++) 
        {
            if (playerBoard[itr->x][itr->y].flag != true  && !playerBoard[itr->x][itr->y].uncovered)
            {
                int remCovered = rowDimension*colDimension - numUncoveredTiles-numFlaggedTiles;
                int flagProb = totalMines-numFlaggedTiles / remCovered;

                if (enumerateMaxProb(itr->x, itr->y) < flagProb)
                {
                    k = itr->x;
                    l = itr->y;
                    break;
                }
                else
                {
                    remnants.insert({enumerateMaxProb(itr->x, itr->y), playerBoard[itr->x][itr->y]});
                    //cout << remnants.size() << endl;
                }
                                
            }
        }
    }
    //cout << "K WASN'T -1" << endl;

    if (k == -1)
    {
        //cout << " FAIL1";
        if (remnants.size() > 0)
        {
            //cout << remnants.begin()->first << "vs " << (remnants.begin()++)->first << endl;
            k = remnants.begin()->second.x;
            l = remnants.begin()->second.y;

        }
        else
        {
            int x = rand() % colDimension;
            int y = rand() % rowDimension;

            while (playerBoard[x][y].uncovered || playerBoard[x][y].flag)
            {
                x = rand() % colDimension;
                y = rand() % rowDimension;
            }

            k = x;
            l = y;
        }


    }

    //cout << "K WASN'T -1 AGAIN" << endl;
    return playerBoard[k][l];
}


int MyAI::enumerateMaxProb(int x, int y)
{
    float max = 0;
    for (int dx = -1; dx <= 1; ++dx) 
    {
        for (int dy = -1; dy <= 1; ++dy) 
        {
            int nx = x + dx;
            int ny = y + dy;
            if (nx >= 0 && nx < colDimension && ny >= 0 && ny < rowDimension && playerBoard[nx][ny].flag && playerBoard[nx][ny].uncovered) 
            {
                if ((playerBoard[nx][ny].effectiveLabel + playerBoard[nx][ny].effectiveModifier) / playerBoard[nx][ny].numCoveredNeighbors > max )
                {
                    max = (playerBoard[nx][ny].effectiveLabel + playerBoard[nx][ny].effectiveModifier) / playerBoard[nx][ny].numCoveredNeighbors;
                }
            }
        }
    }
    return max;

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

    //// cerr << "Effective label of " << x+1 << " " << y+1 << ": " << number - numMarkedNeighbors << endl;
    return playerBoard[x][y].effectiveLabel - numMarkedNeighbors + playerBoard[x][y].effectiveModifier;
}

int MyAI::getNumCoveredNeighbors(int x, int y)
{
    int numCoveredNeighbors = 0;
    for (int dx = -1; dx <= 1; ++dx)
    {
        for (int dy = -1; dy <= 1; ++dy) 
        {
            int nx = x + dx;
            int ny = y + dy;
            if (nx >= 0 && nx < colDimension && ny >= 0 && ny < rowDimension && !(dx == 0 && dy == 0) && !playerBoard[nx][ny].uncovered) 
            {
                //// cerr << "Unmarked: " << nx+1 << " " << ny+1 << endl;
                ++numCoveredNeighbors;
            }
        }
    }
    //cerr << "Num unmarked neighbors around " << x+1 << " " << y+1 << ": " << endl;
    return numCoveredNeighbors;

}


int MyAI::getNumMarkedNeighbors(int x, int y)
{
    int numMarkedNeighbors = 0;
    for (int dx = -1; dx <= 1; ++dx)
    {
        for (int dy = -1; dy <= 1; ++dy) 
        {
            int nx = x + dx;
            int ny = y + dy;
            if (nx >= 0 && nx < colDimension && ny >= 0 && ny < rowDimension && !(dx == 0 && dy == 0) && playerBoard[nx][ny].flag && !playerBoard[nx][ny].uncovered) 
            {
                //// cerr << "Unmarked: " << nx+1 << " " << ny+1 << endl;
                ++numMarkedNeighbors;
            }
        }
    }
    //// cerr << "Num unmarked neighbors around " << x+1 << " " << y+1 << ": " << numUnmarkedNeighbors << endl;
    return numMarkedNeighbors;

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
                //// cerr << "Unmarked: " << nx+1 << " " << ny+1 << endl;
                ++numUnmarkedNeighbors;
            }
        }
    }
    //// cerr << "Num unmarked neighbors around " << x+1 << " " << y+1 << ": " << numUnmarkedNeighbors << endl;
    return numUnmarkedNeighbors;
}

// Mark all unmarked neighbors of a tile as mines
vector<MyAI::Tile> MyAI::markUnmarkedNeighbors(int x, int y) 
{
    vector<MyAI::Tile> markedNeighbors;
    for (int dx = -1; dx <= 1; ++dx) 
    {
        for (int dy = -1; dy <= 1; ++dy) 
        {
            int nx = x + dx;
            int ny = y + dy;
            
            if (nx >= 0 && nx < colDimension && ny >= 0 && ny < rowDimension && !(dx == 0 && dy == 0) && !playerBoard[nx][ny].flag && !playerBoard[nx][ny].uncovered) 
            {  
                //// cerr << "Marking " << nx+1 << " " << ny+1 << endl;
                playerBoard[nx][ny].flag = true;
                // Update the number of flagged tiles
                ++numFlaggedTiles;

                markedNeighbors.push_back(Tile(nx, ny));
            }
        }
    }

    return markedNeighbors;
}

void MyAI::decrementCoveredNeighborValue(int x, int y)
{
    for (int dx = -1; dx <= 1; ++dx) 
    {
        for (int dy = -1; dy <= 1; ++dy) 
        {
            int nx = x + dx;
            int ny = y + dy;
            
            if (nx >= 0 && nx < colDimension && ny >= 0 && ny < rowDimension && !(dx == 0 && dy == 0) && !playerBoard[nx][ny].flag && !playerBoard[nx][ny].uncovered) 
            {  
                playerBoard[nx][ny].numCoveredNeighbors -= 1;
                // cerr << nx << " " << ny << ": " << playerBoard[nx][ny].numCoveredNeighbors << endl;
            }
        }
    }
}

bool MyAI::contains(vector<Tile> a, Tile b)
{

    for (int i = 0; i < a.size(); i++)
    {
        //// cerr << " {a[" << i << "].x: " << a[i].x << " a[" << i << "].y: " << a[i].y << "}" ;

        if (a[i].x == b.x && a[i].y == b.y)
        {
            return true;
        }
        
    }
    //// cerr << endl;
    return false;
}

int MyAI::tileOriginDiff(Tile exam)
{

    //return how many neighbors were covered at initiation of program
    if (exam.x == 0 || exam.x == colDimension - 1)
        {
            if (exam.y == 0 || exam.y == rowDimension - 1)
            {
                return 3;
            }
            else
            {
                return 5;
            }
        }
        if (exam.y == 0 || exam.y == rowDimension - 1)
        {
            if (exam.x == 0 || exam.x == colDimension - 1)
            {
                return  3;
            }
            else
            {
                return 5;
            }
        }
        
    return 8;
}

bool MyAI::tileComp(Tile a, Tile b)
{
    if (a.numCoveredNeighbors == b.numCoveredNeighbors)
    {
        return a.effectiveLabel < b.effectiveLabel;
    }
    else
    {
        return a.numCoveredNeighbors < b.numCoveredNeighbors;
    } 
}
MyAI::Tile* MyAI::getIthNeighbor(Tile a, int i)
{
    for (int dx = -1; dx <= 1; ++dx) 
    {
        for (int dy = -1; dy <= 1; ++dy) 
        {
            int nx = a.x + dx;
            int ny = a.y + dy;

            
            if (nx >= 0 && nx < colDimension && ny >= 0 && ny < rowDimension && !(dx == 0 && dy == 0) && !playerBoard[nx][ny].flag && !playerBoard[nx][ny].uncovered) 
            {  
                if (i == 0)
                {
                    return &playerBoard[nx][ny];
                }

            }
            i -= 1;
        }
    }
    return nullptr;

}

void MyAI::updateFrontiers()
{
    // Clear previous frontiers
    coveredFrontier.clear();
    uncoveredFrontier.clear();

    // Iterate over the board
    for (int x = 0; x < colDimension; ++x)
    {
        for (int y = 0; y < rowDimension; ++y)
        {
            // If the tile is covered and has at least one uncovered neighbor, add it to the covered frontier
            if (!playerBoard[x][y].uncovered && hasUncoveredNeighbor(x, y))
            {
                //cout << "Adding: " << x + 1<< ", " << y + 1<< " " << playerBoard[x][y].uncovered << endl;
                coveredFrontier.insert(playerBoard[x][y]);

                if (!uncoveredFrontier.empty())
                    uncoveredFrontier.erase(playerBoard[x][y]);
            }
            // If the tile is uncovered and has at least one covered neighbor, add it to the uncovered frontier
            else if (playerBoard[x][y].uncovered && hasCoveredNeighbor(x, y))
            {
                uncoveredFrontier.insert(playerBoard[x][y]);

                if (!coveredFrontier.empty())
                    coveredFrontier.erase(playerBoard[x][y]);
            }

            // if a tile is uncovered, it's removed from the covered frontier
            if (playerBoard[x][y].uncovered && coveredFrontier.find(playerBoard[x][y]) != coveredFrontier.end() && !coveredFrontier.empty())
            {
                coveredFrontier.erase(playerBoard[x][y]);
            }
        }
    }

    // set<Tile>::iterator itr;
    // for (itr = uncoveredFrontier.begin(); itr != uncoveredFrontier.end(); itr++) 
    // {
    //     cout << "Uncovered frontier: " << itr->x + 1<< ", " << itr->y + 1<< endl;
    // }

    // for (itr = coveredFrontier.begin(); itr != coveredFrontier.end(); itr++) 
    // {
    //     cout << "Covered frontier: " << itr->x +1 << ", " << itr->y + 1<< endl;
    // }
}

bool MyAI::hasUncoveredNeighbor(int x, int y)
{
    // Iterate over neighbors
    for (int dx = -1; dx <= 1; ++dx)
    {
        for (int dy = -1; dy <= 1; ++dy)
        {
            int nx = x + dx;
            int ny = y + dy;
            if (nx >= 0 && nx < colDimension && ny >= 0 && ny < rowDimension &&
                playerBoard[nx][ny].uncovered && !playerBoard[x][y].uncovered)
            {
                return true; // Found an uncovered neighbor
            }
        }
    }
    return false; // No uncovered neighbors found
}

bool MyAI::hasCoveredNeighbor(int x, int y)
{
    // Iterate over neighbors
    for (int dx = -1; dx <= 1; ++dx)
    {
        for (int dy = -1; dy <= 1; ++dy)
        {
            int nx = x + dx;
            int ny = y + dy;
            if (nx >= 0 && nx < colDimension && ny >= 0 && ny < rowDimension &&
                !playerBoard[nx][ny].uncovered)
            {
                return true; // Found a covered neighbor
            }
        }
    }
    return false; // No covered neighbors found
}