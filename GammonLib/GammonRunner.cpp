#include "GammonRunner.hpp"
#include "CommonTypes.hpp"
#include "Move.hpp"
#include "MoveLawyer.hpp"
#include "PlayExplorer.hpp"


#include <chrono>
#include <ctime>
#include <regex>

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
		result &= m_state.movePiece(m.first, m.second, p.color);
	}
	if (!result)
	{
		m_state = tmp;
		cout << "Invalid moves. Restoring state to pre-move." <<endl;
	}
	return result;
}

void GammonRunner::runGame()
{
	const Color humanColor = Color::WHITE;

	while(1)
	{
		cout << m_state.toDebugStr() << endl;
		cout << m_state.toPrettyStr() << endl;

		pair<int, int> diceRoll = getDiceRoll();
		Color currentPlayer = getCurrentPlayer();

		Play p;
		p.color = currentPlayer;
		p.state = m_state;

		if (humanColor == currentPlayer)
		{
			while (!getUserMoves(p, diceRoll))
			{
				cout << "Invalid move string. Please retry." << endl;
				p.moves.clear();
			}
		}
		else
		{
			resetNodeCtorCount();
			std::chrono::time_point<std::chrono::system_clock> startTime = std::chrono::system_clock::now();
			p = PlayExplorer::findBestPlay(p, diceRoll, 2);
			cout << "\n\n=====RESULT=====\nBest Play:\n" << p.toDebugStr() << endl;

			std::chrono::time_point<std::chrono::system_clock> endTime = std::chrono::system_clock::now();
			double duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime).count();
			float nodesPerMs = getNodeCtorCount() / duration;
			cout << "Total time: " << duration / 1000 << "s Speed: " << nodesPerMs << " nodes/ms" <<endl <<endl;
		}

		if (!processMove(p))
		{
			throw std::runtime_error("Failed to processMove()");
		}
		//swap player turn
		m_currentPlayer = (Color)((int)currentPlayer*-1);
	}
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
	8/4 6/4
	6/4(3) 13/11
	bar/22 17/9
	5/off 2/off
	*/

	cout << "Please enter moves for " << (p.color == Color::WHITE ? "White" : "Black") << ": ";
	string str;
	getline(cin, str);

	auto words_begin = std::sregex_iterator(str.begin(), str.end(), MOVE_REGEX);
	auto words_end = std::sregex_iterator();

	if (words_begin == words_end)
	{
		return false;
	}

	for (std::sregex_iterator i = words_begin; i != words_end; ++i)
	{
		std::smatch match = *i;

		Move m;
		m.first = moveToIndex(match[1]);
		m.second = moveToIndex(match[2]);

		int moveCount = 1;
		if (match[4].length())
		{
			moveCount = stoi(match[4].str());
		}

		for (int i = 0; i < moveCount; i++)
		{
			p.moves.push_back(m);
		}
	}

	//verify that the moves are possible given the dice roll?
	vector<int8_t> diceVector = MoveLawyer::getDiceFromRoll(diceRoll);
	for (const Move& m : p.moves)
	{
		if (m.first >= 0 && m.second >= 0)
		{
			int8_t delta = abs(m.second - m.first);
			auto it = std::find(diceVector.begin(), diceVector.end(), delta);
			if (it != diceVector.end()) {
			  diceVector.erase(it);
			}
			else
			{
				cout << "Could not find die matching move.\n";
				return false;
			}
		}
	}

	//the first N moves must be from BUMP, where N is number of bumped pieces.
	for (int i = 0; i < p.state.getBumpedCount(p.color); i++)
	{
		if (i < p.moves.size() && p.moves[i].first != Special::BUMP)
		{
			cout << "Gotta moved your bumped stuff.\n";
			return false;
		}
	}

    return true;
}