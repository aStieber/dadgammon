#pragma once
#include <stdexcept>
#include <utility>
#include <stdint.h>
#include <vector>
#include <map>
#include <unordered_set>
#include <string>
#include <memory>
#include <limits>
#include <iostream>
#include <iomanip>
#include <string>
#include <sstream>
#include <bitset>
#include <atomic>

enum Color {BLACK=-1, WHITE=1, NONE=0};

inline std::ostream& operator<<(std::ostream& os, Color c) {
    switch (c) {
        case Color::WHITE:
            os << "White";
            break;
        case Color::BLACK:
            os << "Black";
            break;
        case Color::NONE:
            os << "None";
            break;
        default:
            os << "UNKNOWN COLOR: " << c;
            break;
    }
    return os;
}