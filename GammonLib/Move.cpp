#include "Move.hpp"

string Play::toDebugStr() const
{
	ostringstream ss;

	ss << "Color: " << (color == Color::WHITE ? "White" : "Black") << " Dice: ";
	for (const int& die : dice)
	{
		ss << die;
	}
	ss << " Moves(1-): ";
	for (const Move& m : moves)
	{
		ss << "{" << (int)m.first+1 << "," << (int)m.second+1 << "} ";
	}
	ss << "\tState: " << state.toDebugStr() <<endl;

	return ss.str();
}