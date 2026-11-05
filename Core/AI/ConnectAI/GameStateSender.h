#pragma once

#include <cstdint>
#include <vector>

// 현재 보드 상태를 서버로 전송하는 클래스
class GameStateSender
{
public:
	GameStateSender() = delete;

	// 현재 게임 상태(보드 + 현재 조각 + 홀드 조각 + 다음 조각들)를 서버로 전송하는 메소드
	static void Send(class Tetris& tetris);

	// 서버 연결 전에 전송하지 못한 메시지를 연결 성공 후 전송하는 메소드
	static void Flush();

private:
	static std::vector<uint8_t> pendingMessage;	// 서버 연결 전에 전송하지 못한 메시지
};
