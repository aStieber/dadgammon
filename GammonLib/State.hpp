#ifndef _STATE_
#define _STATE_
#include "CommonTypes.hpp"


struct State
{
	//array of size 24. White's farthest at 0, black's at 23.
	//white is +, black is -
	//thus, starting array begins [2, 0, 0, 0, 0, -6, 0, -3...]

	//additionally, I need Storage for bumped tiles for black and white.

	//https://en.wikipedia.org/wiki/Backgammon_notation

	State();

	bool movePiece(int8_t start, int8_t end, const Color& turn);


	int getBumpedCountConst(const Color& c) const;

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

	const std::vector<int>& getBoard() const { return m_board; } ;
	static const int getBoardSize() { return 24; };

	
	//the big one. white+/black-.
	float calculateScore() const;


private:
	bool moveBumpedPiece(int8_t end, const Color& turn);
	bool placePiece(int8_t end, const Color& turn);

	inline int& getBumpedCount(const Color& c);


	std::vector<int> m_board; //0-indexed, size 24

	int m_blackBumpedCount;
	int m_whiteBumpedCount;
};

#endif