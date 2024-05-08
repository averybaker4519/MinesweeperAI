// ======================================================================
// FILE:        MyAI.hpp
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

#ifndef MINE_SWEEPER_CPP_SHELL_MYAI_HPP
#define MINE_SWEEPER_CPP_SHELL_MYAI_HPP

#include "Agent.hpp"
#include <iostream> // temporary use
#include <vector>
#include <map>
#include <set>
#include <algorithm>

using namespace std;

class MyAI : public Agent
{
public:
    MyAI ( int _rowDimension, int _colDimension, int _totalMines, int _agentX, int _agentY );

    Action getAction ( int number ) override;


    // ======================================================================
    // YOUR CODE BEGINS
    // ======================================================================

    // taken from World.hpp
    struct Tile
    {
        bool uncovered  = false; // the tile uncovered or not
        bool flag       = false; // the tile has been flag or not
        int  number     = 0;     // records number of bombs around
    };

    vector<vector<Tile>> playerBoard;
    vector<Tile> tilesToUncover;

    int numUncoveredTiles;
    int numFlaggedTiles;

    Agent::Action ruleOfThumb(int number);

    int calculateEffectiveLabel(int x, int y, int number);
    int getNumUnmarkedNeighbors(int x, int y);
    void markUnmarkedNeighbors(int x, int y);

    // ======================================================================
    // YOUR CODE ENDS
    // ======================================================================
};

#endif //MINE_SWEEPER_CPP_SHELL_MYAI_HPP
