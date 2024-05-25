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
#include <math.h>
#include <climits>

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
    class Tile
    {
        public:
            int x;
            int y;        
            bool uncovered  = false; // the tile uncovered or not
            bool flag       = false; // the tile has been flag or not
            int effectiveLabel = -1;
	        int effectiveModifier = 0;
            int numCoveredNeighbors = 8;

            Tile()
            {

            }

            Tile(int ax, int ay)
            {
                x = ax;
                y = ay;
            }

            bool comp(Tile a, Tile b)
            {
                return (a.x == b.x && a.y == b.y);
            }
    };

    vector<vector<Tile>> playerBoard;
    vector<Tile> tilesToUncover;

    //unordered_map<TileLoc, int> tilesToUncover;

    int numUncoveredTiles;
    int numFlaggedTiles;
    int lastX, lastY;

    Agent::Action ruleOfThumb(int number);
    Agent::Action BasicHeuristic(int number);

    int calculateEffectiveLabel(int x, int y, int number);
    int getNumCoveredNeighbors(int x, int y);
    int getNumMarkedNeighbors(int x, int y);
    int getNumUnmarkedNeighbors(int x, int y);
    void decrementCoveredNeighborValue(int x, int y);
    vector<Tile> markUnmarkedNeighbors(int x, int y);
    int originDiff(int x, int y);
    bool contains(vector<Tile> a, Tile b);

    // ======================================================================
    // YOUR CODE ENDS
    // ======================================================================
};

#endif //MINE_SWEEPER_CPP_SHELL_MYAI_HPP
