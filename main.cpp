//~/projects/dadgammon/build$ cmake .. & cmake --build . && ./dadgammon
#include <iostream>
#include <string>

#include "GammonRunner.hpp"


using namespace std;

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

int main()
{
	std::cout << "\n=====\nDadgammon\n=====\nhttps://en.wikipedia.org/wiki/Backgammon_notation\n=====\n\n";

	GammonRunner g(false);
	//GammonEngine engine;
	//cpu:black, player:white

	while(1)
	{
		Color currentPlayer = g.getCurrentPlayer();

		Play p;//());//getDiceRoll());
		p.color = currentPlayer;


		if (currentPlayer == Color::WHITE)
		{
			//t.addDice({1, 3}); //overwrite

			//t.addMove(pair8_t{18, 19});
			//t.addMove(pair8_t{16, 19});
		}
		else 
		{
			//m = engine.getMove(g);
		}
		if (!g.processMove(p))
		{
			throw std::runtime_error("Failed to processMove()");
		}
	}


	return 0;
}

