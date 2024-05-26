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

    tilesToUncover = vector<Tile>();
    dangerFrontier = map<Tile, int, MyAI::classComp>();

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


            cout << x << " " << y << ": " << playerBoard[x][y].numCoveredNeighbors << endl;
        }
    }


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
        //cout << "NUM OF uncovered TILES TOTAL: " << numUncoveredTiles << endl;
        playerBoard[agentX][agentY].effectiveLabel = number;
        return ruleOfThumb(playerBoard[agentX][agentY].effectiveLabel); 
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
    //cout << agentX << " " << agentY << "sizeof todo" << tilesToUncover.size() << "number: " << number << endl;
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
                //cout << nx << " vs " << colDimension << ", " << ny << " vs " << rowDimension;
		        
                if (nx >= 0 && nx < colDimension && ny >= 0 && ny < rowDimension && !(dx == 0 && dy == 0)) 
                {
                    //cout << "yes" << endl;
                    playerBoard[nx][ny].numCoveredNeighbors -= 1;

                    if (!playerBoard[nx][ny].flag && !playerBoard[nx][ny].uncovered && tileOriginDiff(playerBoard[nx][ny]) == playerBoard[nx][ny].numCoveredNeighbors)
                    {

                        //cout << "UNCOVERING ALL NEIGHBORS AROUND(adding to queue) " << agentX << " " << agentY << ": " << nx << " " << ny << endl;
                        tilesToUncover.push_back( playerBoard[nx][ny]);


                        //removed subtracting effective label here
                    }
                }
            }
        }
        
        // TODO: guessing logic
        if (tilesToUncover.empty())
        {
            
        }
        else
        {
            Tile nextTile = tilesToUncover.back();            
            tilesToUncover.pop_back();

            //cout << "revealing" << nextTile.x << " vs " << colDimension << ", " << nextTile.y << " vs " << rowDimension << endl;
            contains(tilesToUncover, Tile(-1, -1));

            while (playerBoard[nextTile.x][nextTile.y].uncovered) //get next covered tile (ik, )
            {
                // std::cout << "failed at " << nextTile.x << ", " << nextTile.y << "- remaining" << tilesToUncover.size() << std::endl;

                tilesToUncover.pop_back();
                Tile nextTile = tilesToUncover.back();
            }
            //cout << "revealing" << nextTile.x << " vs " << colDimension << ", " << nextTile.y << " vs " << rowDimension << endl;

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
        return BasicHeuristic(number);
    }

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

                if (tileOriginDiff(playerBoard[nx][ny]) == playerBoard[nx][ny].numCoveredNeighbors+1)
                {
                    dangerFrontier[playerBoard[nx][ny]] = 0;
                }
            }
        }
    }

    if (playerBoard[agentX][agentY].effectiveLabel + playerBoard[agentX][agentY].effectiveModifier == getNumCoveredNeighbors(agentX, agentY))
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
                    if (nx >= 0 && nx < colDimension && ny >= 0 && ny < rowDimension && !(dx == 0 && dy == 0) && !playerBoard[nx][ny].flag && !playerBoard[nx][ny].uncovered) 
                    {
                        playerBoard[nx][ny].effectiveModifier--;

                        if (playerBoard[nx][ny].effectiveLabel + playerBoard[nx][ny].effectiveModifier == 0)
                        {
                            tilesToUncover.push_back(playerBoard[nx][ny]);
                        }

                        //TODO : remove from dangerFrontier, if it was previously in
                    }
                }
            }
        }

        // frontier stuff
    }


    //TODO: if tilesToUncover empty, else


    else
    {
        //balancing heuristic: against # mines remaining / totalCovered - # flags (but how to distinguish frontier from wilds)
        //guess candidate = from .begin()

        //run guesswork (up to..?)

        // if minimum element is > % of random, and exhausted some small sample of searching in this space:

            //just choose random




        // guess logic
    }


    return {LEAVE, -1, -1};
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
            if (nx >= 0 && nx < colDimension && ny >= 0 && ny < rowDimension && !(dx == 0 && dy == 0) && playerBoard[nx][ny].uncovered) 
            {
                //cout << "Unmarked: " << nx+1 << " " << ny+1 << endl;
                ++numCoveredNeighbors;
            }
        }
    }
    //cout << "Num unmarked neighbors around " << x+1 << " " << y+1 << ": " << numUnmarkedNeighbors << endl;
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
                //cout << "Unmarked: " << nx+1 << " " << ny+1 << endl;
                ++numMarkedNeighbors;
            }
        }
    }
    //cout << "Num unmarked neighbors around " << x+1 << " " << y+1 << ": " << numUnmarkedNeighbors << endl;
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
                //cout << "Unmarked: " << nx+1 << " " << ny+1 << endl;
                ++numUnmarkedNeighbors;
            }
        }
    }
    //cout << "Num unmarked neighbors around " << x+1 << " " << y+1 << ": " << numUnmarkedNeighbors << endl;
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
                //cout << "Marking " << nx+1 << " " << ny+1 << endl;
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
                cout << nx << " " << ny << ": " << playerBoard[nx][ny].numCoveredNeighbors << endl;
            }
        }
    }
}

bool MyAI::contains(vector<Tile> a, Tile b)
{

    for (int i = 0; i < a.size(); i++)
    {
        //cout << " {a[" << i << "].x: " << a[i].x << " a[" << i << "].y: " << a[i].y << "}" ;

        if (a[i].x == b.x && a[i].y == b.y)
        {
            return true;
        }
        
    }
    //cout << endl;
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
MyAI::Tile MyAI::getIthNeighbor(Tile a, int i)
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
                    return playerBoard[nx][ny];
                }

            }
            i -= 1;
        }
    }

}


// //TODOS: playerboard for guessFrom to play w/
// //TODOS: way for guessfrom to know what last operation was
// //TODOS: way to cut down upon things to be input: but dont want to repeat tiles -- add adj covered tiles to "to assign" somehow?
// mapSol MyAI::guessFrom(Tile start, int depth=0, Tile lastConstraint, vector<vector<Tile>> copyBoard, map<Tile, bool> Assignment)
// {
//     //TODO check that current assignment is valid (satisfy known adjacent constraints? )


//     //TODO check adjacent tiles for a num > 0 (there must be one, choose w smallest numcovered) = ConstraintTile
        
//         //TODO get permutations for assignments of ConstraintTile: minor recursive 



//     //TODO at a certain depth, just have to return the map
//         //else, whether to continue is whether current/last tile assigned was covered or not
// }

// list<map<Tile, bool>> getPermutations(Tile cur, int effectiveLabel, map<Tile, bool> Assignment)
// {
//     if (effectiveLabel == 0)
//     {
//         //set others remaining in cur neighborhood to Assignment = 0 [no mine]

//         return Assignment;

//     }
//     else
//     {
//         list out = list()
//         for (int i = 0; i < cur.numCoveredNeighbors; i++)
//         {

//             getPermutations(cur, effectiveLabel - 1, Assignment);             //TODO perform assignment inplace in function call?
//             // TODO add to out

//         }
//         return out;

//         //assign to one remaining
//     }
// }

//Return the first neighbor from southeast most clockwise


// struct mapSol {
//     map Mapping;
//     int sz
// }
// ======================================================================
// YOUR CODE ENDS
// ======================================================================
