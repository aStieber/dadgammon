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

	void runGame();

private:

	bool getUserMoves(Play& p, const pair<int, int>& diceRoll);
	bool isGameOver();
	Color getCurrentPlayer() { return m_currentPlayer; };

	bool processMove(Play& p);


	State m_state;	
	Color m_currentPlayer=Color::BLACK;
};