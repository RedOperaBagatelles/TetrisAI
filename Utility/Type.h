#pragma once

#include <cstddef>
#include <cstdint>

#if _DEBUG
using map_size = int;
using low_uint = unsigned int;
#else
using map_size = __int8;
using low_uint = unsigned char;
#endif

// 게임 구분 관련 타입
using GameCapacity = std::uint16_t;                               // 게임 생성 순서대로 1부터 매겨지는 식별자 (uint16 → 1~65535, 0은 미할당/고갈 상태)
inline constexpr GameCapacity invalidGameID = 0;                  // 잘못된 GameID를 나타내는 상수 (0은 미할당/고갈 상태)
inline constexpr std::size_t gameIDSize = sizeof(GameCapacity);   // GameCapacity의 크기 (바이트 단위)

// 한 클라이언트 세션을 구분하는 식별자. gameID는 세션 안에서만 1부터 순번이므로
// 세션이 바뀌어도 서버가 이전 세션의 보드 상태와 명령을 섞지 않도록 함께 전송한다.
using SessionID = std::uint64_t;

struct Position
{
    constexpr Position(int x, int y) : x(x), y(y) { }
    constexpr Position() : x(0), y(0) { }

    bool operator==(const Position& other) const
    {
        return (x == other.x) && (y == other.y);
    }

    map_size x;
    map_size y;
};
