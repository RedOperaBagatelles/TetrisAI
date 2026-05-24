#pragma once

#if _DEBUG
using map_size = int;
using low_uint = unsigned int;
#else
using map_size = __int8;
using low_uint = unsigned char;
#endif

struct Position
{
    constexpr Position(int x, int y) : x(x), y(y) { }
    constexpr Position() : x(0), y(0) { }

    map_size x;
    map_size y;
};
