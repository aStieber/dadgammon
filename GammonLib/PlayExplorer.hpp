#ifndef _PLAYEXPLORER_
#define _PLAYEXPLORER_
#include "CommonTypes.hpp"
#include "Move.hpp"

struct PlayNode;
struct HashPlayNode
{
	__int128 operator()(const shared_ptr<PlayNode>& playNode) const;
};

uint64_t getNodeCtorCount();

bool operator==(const shared_ptr<PlayNode>& x, const shared_ptr<PlayNode>& y);

struct PlayNode
{
	PlayNode(const Play& origin);
	~PlayNode();

	Play play;
	unordered_set<shared_ptr<PlayNode>, HashPlayNode> children;

	float computeScoreRec(int searchDepth);

	bool getComputedScore(float& outScore) const;

	string toDebugStr() const;

private:
	bool hasScoreBeenComputed = false;
	float computedScore = 0;
};

struct PlayExplorer
{
	static Play findBestPlay(const Play& origin, const std::pair<int, int>& roll, int searchDepth=1);

private:
};


#endif