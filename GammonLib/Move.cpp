#include "Move.hpp"

string Play::toDebugStr() const
{
	ostringstream ss;

	ss << "Color: " << (color == Color::WHITE ? "White" : "Black") << " Dice: ";
	for (const int& die : dice)
	{
		if (die > 0)
			ss << die;
	}
	ss << " Moves: ";
	for (const Move& m : moves)
	{
		if (m.first == 0 && m.second == 0)
		ss << "{" << (int)m.first+1 << "," << (int)m.second+1 << "} ";
	}
	ss << "\tState: " << state.toDebugStr() <<endl;

	return ss.str();
}

int Play::getMoveCount() const
{
	for (int i = 0; i < sizeof(moves); i++)
	{
		if (moves[i].first == 0 && moves[i].second == 0)
		{
			return i;
		}
	}
	return 4;
}