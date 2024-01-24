#include "State.hpp"
#include "Move.hpp"
#include <cmath>


using namespace std;

const int BOARD_LENGTH = 24;
State::State()
{
	m_board = std::vector<int>(BOARD_LENGTH);

	//white/+
	m_board[0] = 2;
	m_board[11] = 5;
	m_board[16] = 3;
	m_board[18] = 5;

	//black/-
	m_board[23] = -2;
	m_board[12] = -5;
	m_board[7] = -3;
	m_board[5] = -5;

	m_blackBumpedCount = 0;
	m_whiteBumpedCount = 0;
}

Color State::getWinner() const
{
	int whiteCount = m_whiteBumpedCount;
	int blackCount = m_blackBumpedCount;
	for (size_t i = 0; i < m_board.size(); i++)
	{
		if (m_board[i] > 0) whiteCount += abs(m_board[i]);
		else if (m_board[i] < 0) blackCount += abs(m_board[i]);
	}
	if (!whiteCount) return Color::WHITE;
	if (!blackCount) return Color::BLACK;
	return Color::NONE;
}


int State::getBumpedCountConst(const Color& c) const
{
	switch(c)
	{
	case Color::BLACK: return m_blackBumpedCount;
	case Color::WHITE: return m_whiteBumpedCount;
	default: throw runtime_error("Requested some bullshit");
	}
}

int& State::getBumpedCount(const Color& c)
{
	switch(c)
	{
	case Color::BLACK: return m_blackBumpedCount;
	case Color::WHITE: return m_whiteBumpedCount;
	default: throw runtime_error("Requested some bullshit");
	}
}

bool State::movePiece(int8_t start, int8_t end, const Color& turn)
{
	int8_t delta = (int8_t)turn;  //white = +1, black = -1

	//valid start checks
	if (start == Special::BUMP && getBumpedCount(turn) > 0)
	{
		//moving a bumped back in
		return moveBumpedPiece(end, turn);
	}
	if (end == Special::OFF)
	{
		m_board[start] -= delta;
		return true;
	}
	if (m_board[start] == 0)
	{
		return false;
	}

	if (!placePiece(end, turn)) return false;
	m_board[start] -= 1;

	return true;
}

bool State::moveBumpedPiece(int8_t end, const Color& turn)
{
	if (int& bc = getBumpedCount(turn))
	{
		bc -= 1;

		placePiece(end, turn);
		m_board[end] -= (uint8_t)turn;
		return true;
	}
	throw runtime_error("empty bump count");
}

bool State::placePiece(int8_t end, const Color& turn)
{
	int8_t delta = (int8_t)turn;
	if (isPointEmpty(end) || isPointFriendly(end, turn))
	{
		//moving to an empty point or existing friendly stack
		m_board[end] += delta;
	}
	else if (getPieceCount(end) == 1) //bumping
	{
		m_board[end] = delta;
		getBumpedCount((Color)(delta * -1))++;
	}
	else return false; //more than 1 enemy color, fail turn.
	return true;
}


bool State::isPointEmpty(int8_t index) const
{
	return !m_board[index];
}

int State::getPieceCount(int8_t index) const
{
	return abs(m_board[(size_t)index]);
}

bool State::isColorAllowedToScore(const Color& c) const
{
	//white cares about 0-17
	//black cares about 6-23
	int start = c == Color::WHITE ? 0 : 6;
	for (int i = start; i <= start + 17; i++)
	{
		if (isPointFriendly(i, c))
			return false;
	}
	return true;
}

float State::calculateScore() const
{
	//the higher this number, the better white's position.
	float score = 0;

	//improvements here, can check state of opponent home zone.
	constexpr float BUMPED_PENALTY = 24+3.5;
	score += m_blackBumpedCount * BUMPED_PENALTY;
	score -= m_whiteBumpedCount * BUMPED_PENALTY;

	constexpr float WIN_REWARD = 1000.;
	switch (getWinner())
	{
	case Color::WHITE: return WIN_REWARD;
	case Color::BLACK: return -WIN_REWARD;
	}


	for (size_t i=0; i < m_board.size(); i++)
	{
		const float column = m_board[i];
		if (column == 0) continue;

		const Color color = std::signbit(column) ? Color::BLACK : Color::WHITE;
		float penaltyDirection = (color == Color::WHITE ? -1 : 1);
		float distanceToFinish = color == Color::WHITE ? (24 - i) : (i + 1);

		//penalized for distance to finish.
		score += penaltyDirection * abs(column) * distanceToFinish;

		//penalize single tiles. There's improvements here, not all singles are equal.
		if (abs(column) == 1)
			score += penaltyDirection * 10;
	}



	return score;
}