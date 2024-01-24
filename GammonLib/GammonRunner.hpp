#include <string>

#include "State.hpp"
#include "Move.hpp"


class GammonRunner
{
	//GammonRunner holds the state. GammonRunner receives moves, pushes them into the state.

	//GammonRunner receives a move, calculates new game state.

	//parser for stdin to move

	//GammonMove
	//GameState
public:
	GammonRunner(bool blackStarts);

	bool isGameOver();
	Color getCurrentPlayer() { return m_currentPlayer; };

	bool processMove(Play& p);


private:
	State m_state;	
	Color m_currentPlayer=Color::BLACK;
};