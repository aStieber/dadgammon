#ifndef _STATE_
#define _STATE_
#include "CommonTypes.hpp"

/*

	so it's 24 tiles that can store up to 15 pieces of either color
	
	so I need -15-15

	so that's 5 bits
	5*24 = 120bits + 4 bits for BB and 4 bits for WB = 128bit

ok so long double has 16 bytes
Do you know how I could write a 5-bit signed integer type? 
I can just have masks to pull out the bits I need but...
I guess I just mask the bits into a char. Last 4 go to the last 4 of the char, sign bit goes to the first


*/

 //0-indexed, size 24


struct Board
{
	Board();
	Board(const Board& b) {board = b.board;};

	signed char getBumpedCount(const Color& c) const;
	void modifyBumpedCount(const Color& c, signed char delta);


	signed char operator[](int i) const;
	void modify(int index, signed char delta);
	void insert(int index, signed char value);
	size_t size() const {return 24;} //for looping through points

	bool operator==(const Board& rhs) const;

	const __int128& getRawBoard() const; 
	void setRawBoard(__int128 b) {board = b;}

private:
	__int128 board;
};

struct State
{
	//array of size 24. White's farthest at 0, black's at 23.
	//white is +, black is -
	//thus, starting array begins [2, 0, 0, 0, 0, -6, 0, -3...]

	//additionally, I need Storage for bumped tiles for black and white.

	//https://en.wikipedia.org/wiki/Backgammon_notation

	State();
	State(const State& s);

	bool movePiece(const std::pair<signed char, signed char>& m, const Color& turn);
	bool movePiece(int8_t start, int8_t end, const Color& turn);


	signed char getBumpedCount(const Color& c) const;

	inline bool isPointEmpty(int8_t index) const;
	inline int getPieceCount(int8_t index) const;
	bool isColorAllowedToScore(const Color& c) const;

	inline bool isPointFriendly(int8_t index, const Color& c) const
	{
		if (index >= getBoardSize() || index < 0)
			throw std::exception();
		return ((m_board[index] * (int8_t)c) > 0);
	}


	Color getWinner() const;

	const Board& getBoard() const { return m_board; } ;
	static const int getBoardSize() { return 24; };

	
	//the big one. white+/black-.
	float calculateScore() const;

	std::string toDebugStr() const;
	std::string toMinimalStr() const;
	std::string toPrettyStr() const;

	void debug_setRawBoard(__int128 b) {m_board.setRawBoard(b);}

private:
	bool moveBumpedPiece(int8_t end, const Color& turn);
	bool placePiece(int8_t end, const Color& turn);

	Board m_board;
};

std::string boardToBin(__int128 board);
std::string boardToHex(__int128 board);

#endif