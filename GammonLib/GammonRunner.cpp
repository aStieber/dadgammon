#include "GammonRunner.hpp"
#include "CommonTypes.hpp"

GammonRunner::GammonRunner(bool blackStarts) {
	m_state = State(blackStarts ? Color::BLACK : Color::WHITE);
}

