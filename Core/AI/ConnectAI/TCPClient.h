#pragma once

#include "Core/GameLoop.h"
#include "Core/AI/AIInput.h"

#include <cstdint>
#include <string>
#include <string_view>
#include <vector>
#include <winsock2.h>
#include <thread>
#include <atomic>
#include <memory>

#undef SendMessage

class Tetris;

class TCPClient : public GameLoop
{
public:
	static TCPClient& GetInstance();
	~TCPClient();

	void Initialize() override;
	bool IsConnected() const;									// 서버 연결 결과를 반환하는 메소드

	void SendMessage(const std::string& message) const;			// 서버로 텍스트(JSON) 메시지를 전송하는 메소드
	void SendMessage(const std::vector<uint8_t>& bytes) const;	// 서버로 바이너리(게임 상태) 메시지를 전송하는 메소드

	void StartReceiveThread();									// 서버로부터 메시지 수신 스레드 시작
	void StopReceiveThread();									// 서버로부터 메시지 수신 스레드 중지

	bool IsExecutingActions() const { return isExecutingActions; }	// 서버가 선택한 액션 시퀀스를 실행 중인지 반환

	void SetTetris(Tetris* tetrisPtr);							// Tetris 객체 설정

private:
	std::string GetCheckConnectMessage() const;			// 서버와 연결되었는지 확인하는 메시지를 반환하는 메소드
	bool CheckConnect(std::string_view message) const;	// 서버와 연결되었는지 확인하는 메소드

	void ReceiveLoop();										// 수신 루프
	void ExecuteActions(const std::vector<std::string>& actionNames);

	SOCKET sock = INVALID_SOCKET;

	std::string_view ipAddress = "127.0.0.1";
	int port = 5000;
	bool isConnected = false;

	std::thread receiveThread;
	std::atomic<bool> isReceiving{ false };
	std::atomic<bool> isExecutingActions{ false };

	Tetris* tetris = nullptr;
	std::unique_ptr<AIInput> aiInput;
};