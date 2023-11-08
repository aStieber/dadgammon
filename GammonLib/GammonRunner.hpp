#include <string>

#include "State.hpp"
#include "Move.hpp"


class GammonRunner
{
	//GammonRunner holds the state. GammonRunner receives moves, passes moves + game state to GammonBoy

	//GammonBoy receives a move + a state, calculates new game state.

	//parser for stdin to move

	//GammonMove
	//GameState
public:
	GammonRunner(bool blackStarts);


private:
	State m_state;	
};