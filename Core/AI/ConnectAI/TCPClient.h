#pragma once

#include "Core/GameLoop.h"

#include <string_view>
#include <winsock2.h>

class TCPClient : public GameLoop
{
public:
	static TCPClient& GetInstance();

	void Initialize() override;
	bool IsConnected() const;							// 서버와 연결되었는지 확인하는 메소드

private:
	std::string_view GetCheckConnectMessage() const;	// 서버와 연결되었는지 확인하는 메시지를 반환하는 메소드
	bool CheckConnect(std::string_view message) const;	// 서버와 연결되었는지 확인하는 메소드

	SOCKET sock = INVALID_SOCKET;

	std::string_view ipAddress = "127.0.0.1";
	int port = 5000;
	bool isConnected = false;
};