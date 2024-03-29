#include "State.hpp"
#include "Move.hpp"
#include <cmath>


State::State()
{
	m_board = Board();
}

Color State::getWinner() const
{
	//todo: this can be improved
	int whiteCount = getBumpedCount(Color::WHITE);
	int blackCount = getBumpedCount(Color::BLACK);
	for (size_t i = 0; i < m_board.size(); i++)
	{
		if (m_board[i] > 0) whiteCount += abs(m_board[i]);
		else if (m_board[i] < 0) blackCount += abs(m_board[i]);
	}
	if (!whiteCount) return Color::WHITE;
	if (!blackCount) return Color::BLACK;
	return Color::NONE;
}


signed char State::getBumpedCount(const Color& c) const
{
	switch(c)
	{
	case Color::BLACK: return m_board.getBumpedCount(Color::BLACK);
	case Color::WHITE: return m_board.getBumpedCount(Color::WHITE);
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
		m_board.modify(start, -delta);
		return true;
	}
	if (m_board[start] == 0)
		return false;
	if (std::signbit(m_board[start]) != std::signbit(delta))
		return false;

	if (!placePiece(end, turn)) return false;
	m_board.modify(start, -delta);

	return true;
}

bool State::moveBumpedPiece(int8_t end, const Color& turn)
{
	if (getBumpedCount(turn))
	{
		m_board.modifyBumpedCount(turn, -1);
		placePiece(end, turn);
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
		m_board.modify(end,delta);
	}
	else if (getPieceCount(end) == 1) //bumping
	{
		m_board.insert(end, delta);
		m_board.modifyBumpedCount((Color)(-delta), 1);
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
	score += getBumpedCount(Color::BLACK) * BUMPED_PENALTY;
	score -= getBumpedCount(Color::WHITE) * BUMPED_PENALTY;

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

string State::toDebugStr() const
{
	std::ostringstream ss;
	ss << "WB:" << (int)getBumpedCount(Color::WHITE)<< " BB:" << (int)getBumpedCount(Color::BLACK) 
		<< " Board:" << boardToHex(getBoard().getRawBoard());
	return ss.str();
}

const __int128 POINT_MASK = 0x1F;
//Points are 5 bit signed integers. They occupy 0x00-0x77.
signed char Board::operator[](int i) const
{
	signed char tmp = (board >> (i * 5)) & POINT_MASK;
	if (tmp & 0x10)
		tmp = tmp | 0xE0; //copy sign bit to bits 6-8.
	return tmp;
}

void Board::modify(int index, signed char delta)
{
	//cout << "modify index: " << index << " delta: " <<(int)delta << " current val: " <<  (int)this->operator[](index) << endl;
	//cout << boardToBin(board) << endl;
	insert(index, this->operator[](index) + delta);
}

void Board::insert(int i, signed char value)
{
	int oldVal = (int)this->operator[](i);

	__int128 zeroedMask = ~(POINT_MASK << (i * 5));
	__int128 valueMask = (value & 0xF)//4 value bits
		| ((value & 0x80) >> 0x3); //sign bit moved from first signed char bit to 5th bit
	valueMask <<= (i * 5) ; //move whole assembly to offset

	// cout << "\ni: " << i << " write value: " << (int)value << endl;
	// cout << "board before:\t" << std::bitset<64>(board >> 64) << std::bitset<64>(board) << endl;
	// cout << "zero mask:\t" << std::bitset<64>(zeroedMask >> 64) << std::bitset<64>(zeroedMask) << endl;
	// cout << "value mask:\t" << std::bitset<64>(valueMask >> 64) << std::bitset<64>(valueMask) << endl;

	board = (board & zeroedMask) | valueMask;

	// cout << "board after:\t" << std::bitset<64>(valueMask >> 64) << std::bitset<64>(valueMask) << endl;
	// cout << "read value: " << (int)this->operator[](i) << endl;

	int newVal = (int)this->operator[](i);
	if (this->operator[](i) != value)
	{
		ostringstream ss;
		ss << "Failed to write. write value: " << (int)value << " read value: " << (int)this->operator[](i);
		throw runtime_error(ss.str());
	}
}

const __int128 BUMP_MASK = 0xF;
const int WHITE_BUMPED_OFFSET = 0x79; //0x78-0x7B
const int BLACK_BUMPED_OFFSET = WHITE_BUMPED_OFFSET + 0x4; //0x7C-0x7F
signed char Board::getBumpedCount(const Color& c) const
{
	return (board >> (c == Color::WHITE ? WHITE_BUMPED_OFFSET : BLACK_BUMPED_OFFSET)) & BUMP_MASK;
}

void Board::modifyBumpedCount(const Color& c, signed char delta)
{
	signed char val = getBumpedCount(c) + delta;
	if (val < 0) throw runtime_error("Tried to reduce bump count below 0");

	const int offset = (c == Color::WHITE ? WHITE_BUMPED_OFFSET : BLACK_BUMPED_OFFSET);

	__int128 zeroedMask = ~(BUMP_MASK << offset);
	__int128 valueMask = (val & BUMP_MASK);
	valueMask <<= offset;

	board = (board & zeroedMask) | valueMask;

	if (getBumpedCount(c) != val)
	{
		throw runtime_error("bump write failed");
	}
}

bool Board::operator==(const Board& rhs) const
{
	return board == rhs.board;
}

string boardToHex(__int128 board)
{
	ostringstream ss;
	for (int i = 0; i < 128; i+=4)
	{
		ss << std::hex << ((long long)(board >> i) & 0xF);
	}

	string output = ss.str();
	reverse(output.begin(), output.end());
	return output;
}

string boardToBin(__int128 board)
{
	ostringstream ss;
	ss << "0b" <<std::bitset<64>(board >> 64) << std::bitset<64>(board);
	return ss.str();
}


Board::Board()
{
	//no support for 128-bit integer constants, so have to do this.
	//board = 0x00f0000014030001; 
	//board = (board << 64) | 0xb28000e836000002;

	board = 0x04f0000014030001; 
	board = (board << 64) | 0xb28000e83bff8000;
}

const __int128& Board::getRawBoard() const
{
	return board;
}


const std::string LINE = "                 |\n";
string State::toPrettyStr() const
{
	ostringstream ss;
	ss << 
R"(13 14 15 16 17 18|19 20 21 22 23 24
\/ \/ \/ \/ \/ \/|\/ \/ \/ \/ \/ \/
)";
	for (int i = 12; i < m_board.size(); i++)
	{
		int val = (int)m_board[i];
		if (val == 0)
		{
			ss << "  ";
		}
		else
		{
			ss << abs(val) << ((val > 0) ? "W" : "B");
		}
		ss << ((i == 17) ? "|" : " ");
	}
	ss << endl << LINE << LINE;
	ss << "            " << "WB:" << (int)m_board.getBumpedCount(Color::WHITE) 
				<< " | " << "BB:" << (int)m_board.getBumpedCount(Color::BLACK) 
				<< "\n"<< LINE << LINE;
	for (int i = 11; i >= 0; i--)
	{
		int val = (int)m_board[i];
		if (val == 0)
		{
			ss << "  ";
		}
		else
		{
			ss << abs(val) << ((val > 0) ? "W" : "B");
		}
		ss << ((i == 6) ? "|" : " ");
	}
	ss << endl << 
R"(/\ /\ /\ /\ /\ /\|/\ /\ /\ /\ /\ /\
12 11 10  9  8  7| 6  5  4  3  2  1)";

	return ss.str();
}