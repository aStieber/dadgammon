#ifndef _MOVELAWYER_
#define _MOVELAWYER_
#include "State.hpp"
#include "Move.hpp"
#include "CommonTypes.hpp"
#include "PlayExplorer.hpp"

class MoveLawyer
{
public:
	static void computeOpponentPlays(PlayNode* root);
	static void computePossiblePlaysForRoll(PlayNode* output, const pair<int,int>& roll, bool swapColor=false);


	inline static vector<int8_t> getDiceFromRoll(const pair<int,int>& roll);
	static vector<int> getSourceColumnsForDie(const State& s, Color turn, const int8_t die);


	static void getPossiblePlaysForDice(PlayNode* output, const Play& currentPlay, vector<int8_t> remainingDice);

};

#endif