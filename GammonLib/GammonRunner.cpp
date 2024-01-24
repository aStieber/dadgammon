#include "GammonRunner.hpp"
#include "CommonTypes.hpp"
#include "Move.hpp"
#include "MoveLawyer.hpp"
#include "PlayExplorer.hpp"


GammonRunner::GammonRunner(bool blackStarts)
{
	m_state = State();
	m_currentPlayer = blackStarts ? Color::BLACK : Color::WHITE;
}

bool GammonRunner::isGameOver()
{
	return m_state.getWinner() != Color::NONE;
	
}

bool GammonRunner::processMove(Play& p)
{
/*
	std::vector<Play> plays = MoveLawyer::getLegalPlays(m_state, p.color);
	std::cout << "Found # Plays: " << plays.size() << "\n";

	std::set<int> scores;
	for (const Play& p : plays)
	{
		//std::cout << p.toDebugStr(1);

		std::cout << MoveLawyer::getLegalPlays(p.state, p.color == Color::WHITE ? Color::BLACK : Color::WHITE).size() << "\n";
	}
*/
	Play best = PlayExplorer::findBestPlay(p, {3, 5}, 2);




	exit(0);


	return false;

/*
	bool result = true;
	for (const pair8_t& m : t.moves)
	{
		result &= m_state.movePiece(m.first, m.second, t.turn);
	}
	return result;
*/

	return true;
}

