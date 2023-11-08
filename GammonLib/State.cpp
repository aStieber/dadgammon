#include "State.hpp"

const int BOARD_LENGTH = 24;
State::State(Color currentPlayer) {
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

	m_currentPlayer = currentPlayer;
}

Color State::getWinner() {
	return Color::NONE;
}