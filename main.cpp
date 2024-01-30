//~/projects/dadgammon/build$ cmake .. & cmake --build . && ./dadgammon
#include <iostream>
#include <string>

#include "GammonRunner.hpp"


int main()
{
	std::cout << "\n=====\nDadgammon\n=====\nhttps://en.wikipedia.org/wiki/Backgammon_notation\n=====\n\n";

	GammonRunner g(0);

	g.runGame();

	return 0;
}

