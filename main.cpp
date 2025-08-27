//~/projects/dadgammon/build$ cmake .. & cmake --build . && ./dadgammon | tee output.txt

//cmake .. -DCMAKE_BUILD_TYPE=Debug && cmake --build .
//valgrind --tool=callgrind ./dadgammon
//kcachegrind
#include <iostream>
#include <string>

#include "GammonRunner.hpp"


int main()
{
	std::cout << "\n=====\nDadgammon\n=====\nhttps://en.wikipedia.org/wiki/Backgammon_notation\n=====\n\n";
	GammonRunner g(01);

	g.runGame();
	return 0;
}

