#ifndef _PLAYEXPLORER_
#define _PLAYEXPLORER_
#include "CommonTypes.hpp"
#include "Move.hpp"


static uint64_t ctor = 0, dtor = 0;
struct PlayNode
{
	PlayNode(const Play& origin)
	{
		if (ctor++ > 0 && ctor % 1000000 == 0)
		{
			cout << "PlayNode::ctor #" << ctor << endl;
		}
		play = origin;
	}

	~PlayNode()
	{
		if (dtor++ > 0 && dtor % 1000000 == 0)
		{
			cout << "PlayNode::dtor #" << dtor << endl;
		}
	}

	Play play;
	set<shared_ptr<PlayNode>> children;

	float computeScoreRec(int searchDepth);

	bool getComputedScore(float& outScore);

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