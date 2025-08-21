#include "MoveLawyer.hpp"

using namespace std;

#include <iostream>
#include <string>


constexpr int8_t MAX_ROLL = 6;
void MoveLawyer::computeOpponentPlays(PlayNode* root)
{
	//can this be done at compile time
	vector<pair<int, int>> possibleRolls(21); //6+5+4+3+2+1=21
	vector<pair<int, int>>::iterator it = possibleRolls.begin();


	for (int8_t i = 1; i <= MAX_ROLL; i++)
	{
		for (int8_t j = i; j <= MAX_ROLL; j++)
		{
			*it = pair<int, int>{i, j};
			it++;
		}
	}

	for (const pair<int,int>& roll : possibleRolls)
	{
		computePossiblePlaysForRoll(root, roll, true);
	}
}

void MoveLawyer::computePossiblePlaysForRoll(PlayNode* output, const pair<int,int>& roll, bool swapColor)
{
	//populates the Plays of a playnode.
	const vector<int8_t> dice = getDiceFromRoll(roll);
	Play p_0 = output->play;
	p_0.dice.clear();
	p_0.moves.clear();
	if (swapColor)
	{
		if (p_0.color == Color::NONE) throw runtime_error("Provided a node without color and asked to swap.");
		p_0.color = (Color)((int)p_0.color*-1);
	}
	
	getPossiblePlaysForDice(output, p_0, dice);

	if (dice.size() == 2)
	{
		//reverse dice order, only matters for non-duplicate rolls.
		getPossiblePlaysForDice(output, p_0, {dice[1], dice[0]});
	}
}

int8_t MoveLawyer::getPossiblePlaysForDice(PlayNode* output, const Play& currentPlay, vector<int8_t> remainingDice)
{		   
	//given a State s_1, a die, and a color, generate all possible s_2s.

	int8_t die = remainingDice.back();
	remainingDice.pop_back();

	const Color color = currentPlay.color;
	int8_t numValidPlays = 0;
	
	//for each source column, execute the die action and update the state.
	for (const int& start : getSourceColumnsForDie(currentPlay.state, color, die))
	{
		State tmpState = currentPlay.state;
		int end = start;
		int speculativeColumn = start + (int)color * die;

		
		if (start == Special::BUMP)
		{
			if (color == Color::BLACK)
				end = 24 - die; 
			else
				end = die - 1;
		}
		else if (speculativeColumn >= State::getBoardSize() || speculativeColumn < 0)
		{
			if (!tmpState.isColorAllowedToScore(color))
				continue;
			end = Special::OFF;
		}
		else //normal move
		{
			end = speculativeColumn;
		}

		//kill the branch if the move isn't legal.
		if (!tmpState.movePiece(start, end, color)) 
		{
			continue;
		}

		numValidPlays++;

		//create new Play for children
		Play p = currentPlay;
		p.state = tmpState;
		p.dice.push_back(die);
		p.moves.push_back(Move{start, end});

		if (remainingDice.size())
		{
			//cout << "RECURSING: " << p.toDebugStr() << endl;
			if (0 == getPossiblePlaysForDice(output, p, remainingDice))
			{
				//if there's no further plays possible, we need to save our node as-is.
				output->children.insert(shared_ptr<PlayNode>(new PlayNode(p)));
			}
		}
		//if we've used all the dice
		else if (p.moves.size() == p.dice.size())
		{
			//cout << "INSERTING: " << p.toDebugStr() << endl;
			output->children.insert(shared_ptr<PlayNode>(new PlayNode(p)));
		}
		else 
		{
			ostringstream ss;
			ss << "FUCKED: " << remainingDice.size() << "\n" << p.toDebugStr() << endl;
			throw runtime_error(ss.str());
		}
	}
	return numValidPlays;
}

vector<int> MoveLawyer::getSourceColumnsForDie(const State& s, const Color turn, const int8_t die)
{
	if (s.getBumpedCount(turn))
	{
		return vector<int>{ Special::BUMP };
	}


	vector<int> output;
	for (int i = 0; i < s.getBoardSize(); i++)
	{
		if (s.isPointFriendly(i, turn))
		{
			output.push_back(i);
		}
	}

	return output;
}