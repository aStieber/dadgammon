#pragma once
#include <atomic>
#include <bitset>
#include <cstring>
#include <iomanip>
#include <iostream>
#include <limits>
#include <map>
#include <memory>
#include <set>
#include <sstream>
#include <stdexcept>
#include <stdint.h>
#include <string>
#include <string>
#include <unordered_set>
#include <utility>
#include <vector>

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