#include "PlayExplorer.hpp"
#include "MoveLawyer.hpp"

#include <thread>

Play PlayExplorer::findBestPlay(const Play& origin, const pair<int, int>& roll, int searchDepth)
{
	if (origin.color == Color::NONE)
		throw runtime_error("Invalid Color");
	if (!origin.moves.empty())
		throw runtime_error("findBestPlay() origin already contained moves.");


	//We only care about the score of root's children - its score is not meaningful.
	//We compute the possible moves based on this template roll, then recursively generate opposing team possibilities.

	PlayNode root(origin);

	MoveLawyer::computePossiblePlaysForRoll(&root, roll);
	cout <<"size: " <<  root.children.size()<< "\n"; 

	vector<thread> tasks;
	for (const shared_ptr<PlayNode>& possiblePlay : root.children)
	{
	    tasks.push_back(thread { [possiblePlay, searchDepth]() {
	    	possiblePlay->computeScoreRec(searchDepth);
			cout << possiblePlay->play.toDebugStr();

	    }});
	}

	for (thread& th : tasks)
	{
		th.join();
	}

	const Color color = origin.color;

	shared_ptr<PlayNode> bestChild;
	float bestScore = color * -1 * INT_MAX;


	//find best child.
	for (const shared_ptr<PlayNode>& possiblePlay : root.children)
	{
		float score;
		if (!possiblePlay->getComputedScore(score))
		{
			throw runtime_error("PlayNode didn't have computed score.");
		}

		if ((color == Color::WHITE && bestScore < score) ||
			(color == Color::BLACK && bestScore > score))
		{
			bestChild = possiblePlay;
			bestScore = score;
		}
	}


	if (bestChild.get() == nullptr)
	{
		throw runtime_error("bestChild was null.");
	}

	cout << "The best play for dice {" << roll.first << "," << roll.second << "}: " <<
		bestChild->play.toDebugStr() << " with score " << bestScore << endl;




	return origin;
}

float PlayNode::computeScoreRec(int searchDepth)
{
	float childScoreSum = 0.;
	int childCount = 0;

	if (searchDepth > 0)
	{
		//generate possible opponent moves.
		MoveLawyer::computeOpponentPlays(this);

		childCount = children.size();
		int bestScore = play.color * -1 * INT_MAX;
		shared_ptr<PlayNode> bestChild;

		for (const shared_ptr<PlayNode>& child : children)
		{
			if (play.color == child->play.color) throw runtime_error("Parent and Child are matching colors.");
			
			const int score = child->computeScoreRec(searchDepth - 1);

			if ((play.color == Color::WHITE && bestScore < score) ||
				(play.color == Color::BLACK && bestScore > score))
			{
				bestChild = child;
				bestScore = score;
			}
			childScoreSum += score;
		}
		//prune everything besides the best
		children.clear();
		if (bestChild.get() == nullptr)
		{
			children.insert(bestChild);
		}
	}

	const float myPlayScore = play.state.calculateScore();
	constexpr float DECAY_RATE = 0.1; //DECAY_RATE ^ N, where N in generation number.

	float average = 0;
	if (childCount > 0) average = childScoreSum / childCount;

	hasScoreBeenComputed = true;
	computedScore = myPlayScore + (DECAY_RATE * average);

	return computedScore;
}

bool PlayNode::getComputedScore(float& outScore)
{
	if (hasScoreBeenComputed)
	{
		outScore = computedScore;
		return true;
	}
	return false;
}