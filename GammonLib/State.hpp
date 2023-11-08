#include <vector>
#include "CommonTypes.hpp"

struct State
{
public:
	State(){};
	State(Color startingPlayer);

	//array of size 24. White's farthest at 0, black's at 23.
	//white is +, black is -
	//thus, starting array begins [2, 0, 0, 0, 0, -6, 0, -3...]

	//additionally, I need Storage for bumped tiles for black and white.

	//https://en.wikipedia.org/wiki/Backgammon_notation

	Color getWinner();

	std::vector<int> m_board;

	int m_blackBumpedCount;
	int m_whiteBumpedCount;

	Color m_currentPlayer=Color::BLACK;
};