#include "CommonTypes.hpp"
#include "GammonRunner.hpp"
#include "Move.hpp"
#include "MoveLawyer.hpp"
#include "PlayExplorer.hpp"


#include <chrono>
#include <ctime>
#include <regex>
#include <random>

pair<int, int> getDiceRoll()
{
start:
	cout << "Please enter the dice roll in the format \"XY\": ";
	string str;
	getline(cin, str);
	int input = stoi(str);

	int8_t i = input / 10;
	int8_t j = input % 10;

	if (i < 1 || i > 6 
		|| j < 1 || j > 6)
	{
		printf("Numbers must be between 1-6.\n");
		goto start;
	}
	
	if (i > j) return pair<int, int>{j, i};
	return pair<int, int>{j, i}; 
}

pair<int, int> getRandomDiceRoll()
{
	static random_device r;
 
    // Choose a random mean between 1 and 6
    static default_random_engine e1(r());
    static uniform_int_distribution<int> uniform_dist(1, 6);

    return pair<int,int>{uniform_dist(e1), uniform_dist(e1)};

}

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
	State tmp = m_state;
	bool result = true;
	for (const Move& m : p.moves)
	{
		if (m.first == 0 && m.second == 0)
			continue;
		result &= m_state.movePiece(m.first, m.second, p.color);
	}
	if (!result)
	{
		m_state = tmp;
		cout << "Invalid moves. Restoring state to pre-move." <<endl;
	}
	return result;
}

const bool BOT_GAME = true;
void GammonRunner::runGame()
{
	const Color humanColor = Color::WHITE;

	resetNodeCtorCount();
	uint64_t turnCount = 0;

	while(!isGameOver())
	{
		/*
		__int128 test = (static_cast<__int128>(0x01f00000140317c1ULL) << 64) | 0xc20000f03dd00000ULL;
		m_state.debug_setRawBoard(test);
		
		pair<int, int> diceRoll = pair<int, int>{6, 6};
		Color currentPlayer = Color::WHITE;
		*/

		cout << m_state.toDebugStr() << endl;
		cout << m_state.toPrettyStr() << endl;

		pair<int, int> diceRoll = getRandomDiceRoll();//getDiceRoll();
		Color currentPlayer = getCurrentPlayer();

		cout << "\n\n=====New Turn=====\nDice: " << diceRoll.first << diceRoll.second << " Color: " << (currentPlayer == Color::WHITE ? "White" : "Black") << endl;
		Play p;
		p.color = currentPlayer;
		p.state = m_state;

		if (!BOT_GAME && humanColor == currentPlayer)
		{
			while (!getUserMoves(p, diceRoll))
			{
				cout << "Your input was invalid. Please retry." << endl;
				memset(p.moves, 0, sizeof(p.moves));
			}
		}
		else
		{
			uint64_t startCount = getNodeCtorCount();
			std::chrono::time_point<std::chrono::system_clock> startTime = std::chrono::system_clock::now();

			int searchDepth = 1; //1 = consider opponent moves
			p = PlayExplorer::findBestPlay(p, diceRoll, searchDepth);
			
			cout << "\n=====RESULT=====\nBest Play:\n" << p.toDebugStr() << endl;

			std::chrono::time_point<std::chrono::system_clock> endTime = std::chrono::system_clock::now();
			double duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime).count();
			float nodes = getNodeCtorCount() - startCount;
			float nodesPerSec = nodes/ duration / 1000;
			cout << "Total time: " << duration / 1000 << "s | Nodes: " << nodes << " | Speed: " << std::fixed << std::setprecision(2) << nodesPerSec << "M node/sec\n\n";
		}

		if (!processMove(p))
		{
			throw std::runtime_error("Failed to processMove()");
		}
		//swap player turn
		m_currentPlayer = (Color)((int)currentPlayer*-1);
		turnCount++;

		//return;
	}
	Color winner = m_state.getWinner();
	cout << "\n\n=====  GAME  ====="
		 << "\n=====  OVER  ====="
		 << "\n   Winner: " << m_state.getWinner()
		 << "\n   mean nodes/turn: " << (getNodeCtorCount() / turnCount) << endl;

}

int8_t moveToIndex(string str)
{
	if (str == "off")
		return Special::OFF;
	if (str == "bar") 
		return Special::BUMP;
	return (int8_t)stoi(str) - 1;
}

const regex MOVE_REGEX(R"((\d{1,2}|bar|off)\/(\d{1,2}|bar|off)(\((\d)\))?)");
bool GammonRunner::getUserMoves(Play& p, const pair<int, int>& diceRoll)
{
	/*
	example entries
	8/4 6/4
	6/4(3) 13/11
	bar/22 17/9
	5/off 2/off
	*/

	Play tmpPlay = p;

	cout << "Please enter moves for " << p.color << ": ";
	string str;
	getline(cin, str);

	auto userInputBegin = std::sregex_iterator(str.begin(), str.end(), MOVE_REGEX);

	if (userInputBegin == std::sregex_iterator())
	{
		return false;
	}

	int moveCount = 0;
	for (std::sregex_iterator i = userInputBegin; i != std::sregex_iterator(); ++i)
	{
		std::smatch match = *i;

		Move m;
		m.first = moveToIndex(match[1]);
		m.second = moveToIndex(match[2]);

		int dupeCount = 1;
		if (match[4].length())
		{
			dupeCount = stoi(match[4].str());
		}

		for (int i = 0; i < dupeCount; i++)
		{
			tmpPlay.moves[moveCount] = m;
		}
	}

	//the first N moves must be from BUMP, where N is number of bumped pieces.
	for (int i = 0; i < tmpPlay.state.getBumpedCount(p.color); i++)
	{
		if (i < tmpPlay.getMoveCount() && tmpPlay.moves[i].first != Special::BUMP)
		{
			cout << "Gotta moved your bumped stuff.\n";
			return false;
		}
	}

	//for each move, verify it's possible
	for (Move& m : tmpPlay.moves)
	{
		if (!tmpPlay.state.movePiece(m, tmpPlay.color))
		{
			std::cout <<"The following move was invalid: " << (int)m.first << ", " << (int)m.second;
			return false;
		}
	}

	//verify that there are not more moves possible
	auto rolls = MoveLawyer::getDiceFromRoll(diceRoll);
	if ( tmpPlay.getMoveCount() > rolls.size() )
	{
		cout << "You moved too many pieces for that roll.\n";
		return false;
	}
	else if ( tmpPlay.getMoveCount() < rolls.size() )
	{
		//made less moves than available. This is only illegal if there are available moves.

		//get all possible moves that can be made, use unchanged Play
		PlayNode root(p);
		MoveLawyer::computePossiblePlaysForRoll(&root, diceRoll);
		//check if the current state is among them.

		std::shared_ptr<PlayNode> tmpPlayNode(new PlayNode(tmpPlay));
		if (!root.children.contains(tmpPlayNode))
		{
			cout << "You have available moves.\n";
			return false;
		}
	}

    return true;
}