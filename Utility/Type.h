#pragma once

using map_size = char;
using low_uint = unsigned char;

struct Position
{
    constexpr Position(int x, int y) : x(x), y(y) { }
    constexpr Position() : x(0), y(0) { }

    map_size x;
    map_size y;
};
