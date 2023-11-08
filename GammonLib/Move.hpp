#ifndef _MOVE_
#define _MOVE_
#include <utility>
#include <stdint.h>

class pair8 : public std::pair<int8_t, int8_t>{};

struct Move {
	pair8 dice;

	static const int8_t BUMP = -1, OFF = -2;
	pair8 move1;
	pair8 move2;
};


#endif