#include "Move.hpp"

string Play::toDebugStr() const
{
	ostringstream ss;

	ss << "Color: " << color << " Dice: ";
	for (const int& die : dice)
	{
		ss << die;
	}
	ss << " Moves(0-): ";
	for (const Move& m : moves)
	{
		ss << "{" << (int)m.first << "," << (int)m.second << "} ";
	}
	ss << "\tState: " << state.toDebugStr() <<endl;

	ss << state.prettyPrint() <<endl;
	return ss.str();
}