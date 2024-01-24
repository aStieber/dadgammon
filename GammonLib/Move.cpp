#include "Move.hpp"
#include <sstream>

using namespace std;



string Play::toDebugStr(bool calculateScore) const
{
	ostringstream ss;

	ss << "Color: " << color << "\tDice: ";
	for (const int& die : dice)
	{
		ss << die << " ";
	}
	ss << "\tMoves: ";
	for (const Move& m : moves)
	{
		ss << "{" << (int)m.first << "," << (int)m.second << "} ";
	}
	ss << "\t";

	if (calculateScore)
	{
		ss << "Score: " << state.calculateScore();
	}
	ss << "\n";
	return ss.str();
}