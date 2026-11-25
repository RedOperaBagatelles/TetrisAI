#pragma once

#include "Core/GameLoop.h"
#include "Utility/Type.h"

#include <cstdint>
#include <string>
#include <string_view>
#include <unordered_map>
#include <vector>
#include <winsock2.h>
#include <thread>
#include <atomic>

#undef SendMessage

class Tetris;

class TCPClient : public GameLoop
{
public:
	static TCPClient& GetInstance();
	~TCPClient();

	TCPClient(const TCPClient&) = delete;
	TCPClient& operator=(const TCPClient&) = delete;

	void Initialize() override;
	bool IsConnected() const;													// 서버 연결 결과를 반환하는 메소드

	void SendMessage(const std::string& message) const;							// 서버로 텍스트(JSON) 메시지를 전송하는 메소드
	void SendMessage(const std::vector<uint8_t>& bytes) const;					// 서버로 바이너리(게임 상태) 메시지를 전송하는 메소드

	void StartReceiveThread();													// 서버로부터 메시지 수신 스레드 시작
	void StopReceiveThread();													// 서버로부터 메시지 수신 스레드 중지

	bool IsExecutingActions() const { return isExecutingActions; }				// 서버가 선택한 액션 시퀀스를 실행 중인지 반환

	void SetGames(const std::vector<std::reference_wrapper<Tetris>>& newGames);	// Tetris 객체들을 등록하여 gameID → Tetris* 매핑을 구성하는 메소드

private:
	TCPClient();

	static SessionID GenerateSessionID();					// 클라이언트 세션 ID 생성
	static bool ExtractNumberField(std::string_view message, std::string_view key, std::uint64_t& value);	// JSON에서 숫자/문자열 필드 추출

	std::string GetCheckConnectMessage() const;				// 서버와 연결되었는지 확인하는 메시지를 반환하는 메소드
	bool CheckConnect(std::string_view message) const;		// 서버와 연결되었는지 확인하는 메소드

	void ReceiveLoop();										// 수신 루프
	void ExecuteActions(SessionID commandSessionID, GameCapacity targetGameID, const std::vector<std::string>& actionNames);

	SOCKET sock = INVALID_SOCKET;

	std::string_view ipAddress = "127.0.0.1";
	int port = 5000;
	bool isConnected = false;

	std::thread receiveThread;
	std::atomic<bool> isReceiving{ false };
	std::atomic<bool> isExecutingActions{ false };

	SessionID sessionID = 0;							// 재접속/다른 클라이언트와 구분하기 위한 세션 ID
	std::size_t gameCount = 0;
	std::unordered_map<GameCapacity, Tetris*> games;	// gameID → 실제 게임 객체
};
