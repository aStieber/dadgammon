#ifndef _MOVE_
#define _MOVE_
#include "State.hpp"
#include "CommonTypes.hpp"

using namespace std;

//a Turn consists of 2 Plays, one for each player

//a Play consists of the 2/4 dice rolled, 0-4 Moves which are the start and end indeces (including BUMP/OFF) 
//     of those die uses, and a game state after those moves have been applied.

namespace Special
{
	static const int8_t BUMP = -1, OFF = -2;
}
#define Move pair<int8_t, int8_t>


struct Play
{
	Color color;
	vector<int> dice;
	set<Move> moves;
	State state;

	string toDebugStr(bool calculateScore=false) const;
private:
};

bool operator<(const Play& lhs, const Play& rhs);


struct Turn
{

private:
};

#endif