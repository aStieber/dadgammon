#include "PlayExplorer.hpp"
#include "MoveLawyer.hpp"

#include <thread>


std::atomic_uint64_t* NODE_COUNTER=new atomic_uint64_t;

uint64_t getNodeCtorCount(){
	return *NODE_COUNTER;
}

void resetNodeCtorCount()
{
	*NODE_COUNTER = 0;
}

PlayNode::PlayNode(const Play& _play)
{
	const uint64_t divisor = 10 * 1000 * 1000;
	uint64_t count = NODE_COUNTER->fetch_add(1);
	/*
	if (count % divisor == 0 && count != 0)
	{
		cout << "PlayNode::ctor " << count / 1000000 << "M\r";
		cout.flush();
	}
	*/
	play = _play;
}

PlayNode::~PlayNode(){}

std::mutex MAP_LOCK;
std::map<__int128, float> BOARD_TO_SCORE_MAP;
uint64_t SKIP_COUNT;

Play PlayExplorer::findBestPlay(const Play& origin, const pair<int, int>& roll, int searchDepth)
{
	if (origin.color == Color::NONE)
		throw runtime_error("Invalid Color");
	if (!origin.moves.empty())
		throw runtime_error("findBestPlay() origin already contained moves.");

	//We only care about the score of root's children - root score is not meaningful.
	//We compute the possible moves based on this template roll, then recursively generate opposing team possibilities.
	PlayNode root(origin);

	//create children
	MoveLawyer::computePossiblePlaysForRoll(&root, roll);

	//score the children
	vector<shared_ptr<PlayNode>> rawPlays(root.children.begin(), root.children.end());
	#pragma omp parallel for num_threads(24) 
	for (int i = 0; i < rawPlays.size(); i++)
	{
		rawPlays[i]->computeScoreRec(1 + searchDepth);
	}
	rawPlays.clear();

	//find best child.
	const Color color = origin.color;
	shared_ptr<PlayNode> bestChild;
	float bestScore = color * -1 * INT_MAX;
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

	if (bestChild)
	{
		return bestChild->play;

	}
	cout << "No children, making no moves.\n";
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
		//cout << "size: " << childCount << endl;
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
		if (bestChild)
		{
			children.insert(bestChild);
		}
	}

	const float myPlayScore = play.state.calculateScore();
	constexpr float DECAY_RATE = 0.5; //DECAY_RATE ^ N, where N is generation number.

	float average = 0;
	if (childCount > 0) average = childScoreSum / childCount;

	hasScoreBeenComputed = true;
	computedScore = myPlayScore + (DECAY_RATE * average);

	return computedScore;
}

bool PlayNode::getComputedScore(float& outScore) const
{
	if (hasScoreBeenComputed)
	{
		outScore = computedScore;
		return true;
	}
	return false;
}

string PlayNode::toDebugStr() const
{
	ostringstream ss;

	ss << play.toDebugStr();

	float score = 0;
	if (getComputedScore(score))
	{
		ss << "Score: " << score;
	}

	return ss.str();
}

bool operator==(const shared_ptr<PlayNode>& x, const shared_ptr<PlayNode>& y)
{
    return x->play.state.getBoard().getRawBoard() == y->play.state.getBoard().getRawBoard();
}

__int128 HashPlayNode::operator()(const shared_ptr<PlayNode>& playNode) const
{
	return playNode->play.state.getBoard().getRawBoard();
}
