#include "MoveLawyer.hpp"

using namespace std;

#include <iostream>
#include <string>

class IntermediateStateCacher
{
public:
	IntermediateStateCacher() {};
	bool contains(State& s) 
	{
		return m_set.contains(s.getBoard().getRawBoard());
	}

	void insert(State& s)
	{
		m_set.insert(s.getBoard().getRawBoard());
	}

protected:
	unordered_set<__int128> m_set;

};

constexpr size_t MAX_ROLL = 6;
constexpr size_t TOTAL_ROLLS = (MAX_ROLL * (MAX_ROLL + 1)) / 2;
constexpr auto generatePossibleRolls() {
	std::array<std::pair<int, int>, TOTAL_ROLLS> rolls = {};
	size_t index = 0;

	for (size_t i = 1; i <= MAX_ROLL; ++i) {
		for (size_t j = i; j <= MAX_ROLL; ++j) {
			rolls[index++] = std::make_pair(i, j);
		}
	}
	return rolls;
}
constexpr auto POSSIBLE_OPPONENT_ROLLS = generatePossibleRolls();

void MoveLawyer::computeOpponentPlays(PlayNode* root)
{
	for (const pair<int,int>& roll : POSSIBLE_OPPONENT_ROLLS)
	{
		computePossiblePlaysForRoll(root, roll, true);
	}
}

void MoveLawyer::computePossiblePlaysForRoll(PlayNode* output, const pair<int,int>& roll, bool swapColor)
{
	//populates the Plays of a playnode.
	const vector<int8_t> dice = getDiceFromRoll(roll);
	Play p_0 = output->play;
	memset(p_0.dice, 0, sizeof(p_0.dice));
	memset(p_0.moves, 0, sizeof(p_0.moves));
	if (swapColor)
	{
		if (p_0.color == Color::NONE) throw runtime_error("Provided a node without color and asked to swap.");
		p_0.color = (Color)((int)p_0.color*-1);
	}

	if (dice.size() == 2)
	{
		getPossiblePlaysForDice(output, p_0, dice, std::shared_ptr<IntermediateStateCacher>());
		//reverse dice order, only matters for non-duplicate rolls.
		getPossiblePlaysForDice(output, p_0, {dice[1], dice[0]}, std::shared_ptr<IntermediateStateCacher>()); //todo: this is so wasteful
	}
	else if (dice.size() == 4)
	{
		std::shared_ptr<IntermediateStateCacher> cacher(new IntermediateStateCacher());
		getPossiblePlaysForDice(output, p_0, dice, cacher);
	}
	else
	{
		ostringstream ss;
		ss << "Invalid dice count, wtf: " << dice.size() << endl;
		throw runtime_error(ss.str());
	}

}


uint16_t MoveLawyer::getPossiblePlaysForDice(PlayNode* output, const Play& currentPlay, vector<int8_t> remainingDice, const std::shared_ptr<IntermediateStateCacher>& cacher, uint8_t depth)
{
	//given a State s_1, a die, and a color, generate all possible s_2s.


	/*
	for doubles
	each die has the same options as the previous, plus the possible new space, and minus the possible moved-from space.
	this can be determined by comparing the source columns

	


	this is gonna recurse 4 times. for loop? makes caching easier

	get a list of columns i could move to

	set(each_source_column * set(n,2n,3n,4n))
	for each targetcolumn
		immediately disqualify those with opponent tiles
	*/




	int8_t die = remainingDice.back();
	remainingDice.pop_back();

	const Color color = currentPlay.color;
	uint16_t numValidPlays = 0;

	
	//for each source column, execute the die action and update the state.
	for (const int& start : getSourceColumnsForDie(currentPlay.state, color, die)) //for (int i = numRecursions, don't need to check first column again after this)
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

		//prune the branch if the move isn't legal.
		if (!tmpState.movePiece(start, end, color)) 
		{
			continue;
		}

		numValidPlays++;

		//has this partial state already been explored. Only valid for quad rolls
		if (cacher && cacher->contains(tmpState))
		{
			//state already explored
			continue;
		}

		//create new Play for children
		Play p = currentPlay;
		p.state = tmpState;
		p.dice[depth] = die;
		p.moves[depth] = Move{start, end};
		//std::cout << "Searching state: " << p.toDebugStr();

		if (remainingDice.size())
		{
			if (0 == getPossiblePlaysForDice(output, p, remainingDice, cacher, depth+1))
			{
				//if there's no further plays possible, we need to save our node as-is.
				output->children.insert(shared_ptr<PlayNode>(new PlayNode(p)));
			}
		}
		//if we've used all the dice
		else
		{
			//cout << "INSERTING: " << p.toDebugStr() << endl;
			output->children.insert(shared_ptr<PlayNode>(new PlayNode(p)));
		}

		if (cacher)
			cacher->insert(tmpState);
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