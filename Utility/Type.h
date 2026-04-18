#pragma once

using map_size = unsigned char;
using low_uint = unsigned char;

struct Position
{
    constexpr Position(int x, int y) : x(static_cast<map_size>(x < 0 ? 0 : x)), y(static_cast<map_size>(y < 0 ? 0 : y)) { }
    constexpr Position(map_size x, map_size y) : x(x), y(y) { }
    constexpr Position() : x(0), y(0) { }

    map_size x, y;
};
