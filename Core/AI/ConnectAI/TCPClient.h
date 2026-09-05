#pragma once

#include "Core/GameLoop.h"

#include <cstdint>
#include <string>
#include <string_view>
#include <vector>
#include <winsock2.h>

#undef SendMessage

class TCPClient : public GameLoop
{
public:
	static TCPClient& GetInstance();
	~TCPClient();

	void Initialize() override;
	bool IsConnected() const;									// 서버 연결 결과를 반환하는 메소드

	void SendMessage(const std::string& message) const;			// 서버로 텍스트(JSON) 메시지를 전송하는 메소드
	void SendMessage(const std::vector<uint8_t>& bytes) const;	// 서버로 바이너리(게임 상태) 메시지를 전송하는 메소드

private:
	std::string GetCheckConnectMessage() const;			// 서버와 연결되었는지 확인하는 메시지를 반환하는 메소드
	bool CheckConnect(std::string_view message) const;	// 서버와 연결되었는지 확인하는 메소드

	SOCKET sock = INVALID_SOCKET;

	std::string_view ipAddress = "127.0.0.1";
	int port = 5000;
	bool isConnected = false;
};