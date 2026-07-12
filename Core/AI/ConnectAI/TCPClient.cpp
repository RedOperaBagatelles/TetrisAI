#include "TCPClient.h"

#include <iostream>
#include <string>
#include <thread>
#include <chrono>

#pragma comment(lib, "ws2_32.lib")
#pragma warning(disable:4996)

TCPClient& TCPClient::GetInstance()
{
	static TCPClient instance;

	return instance;
}

void TCPClient::Initialize()
{
	// Windows에서 소캣을 사용하기 위해 WinSock 라이브러리를 초기화
	WSADATA wsa;
	WSAStartup(MAKEWORD(2, 2), &wsa);							// WinSock 2.2 버전으로 초기화

	while (!isConnected)
	{
		sock = socket(AF_INET, SOCK_STREAM, 0);				// TCP 소켓 생성 (AF_INET : IPv4, SOCK_STREAM : TCP [SOCK_DGRAM : UDP])

		// 서버 정보를 저장하는 구조체 초기화
		sockaddr_in server;
		server.sin_family = AF_INET;
		server.sin_port = htons(port);								// htos : 네트워크 표준인 빅엔디안 순서로 변경
		server.sin_addr.s_addr = inet_addr(ipAddress.data());

		// 서버 접속
		if (connect(sock, (sockaddr*)&server, sizeof(server)) == SOCKET_ERROR)
		{
			std::cerr << "Failed to connect to server. Retrying in 1 second...\n";
			closesocket(sock);
			std::this_thread::sleep_for(std::chrono::seconds(1));

			continue;
		}

		// 서버와 연결이 되었는지 확인하는 데이터 전송
		std::string_view message = GetCheckConnectMessage();
		send(sock, message.data(), static_cast<int>(message.size()), 0);

		// 서버로부터 응답 받음
		std::string receivedMessage(1024, '\0');
		int received = recv(sock, receivedMessage.data(), static_cast<int>(sizeof(receivedMessage)) - 1, 0);	// 반환 값 : 수신된 바이트 수

		// 수신된 바이트 수가 0보다 크면 서버와 연결 성공, 그렇지 않으면 실패
		if (received > 0)
		{
			// 수신된 바이트 수만큼 문자열 크기 조정
			receivedMessage.resize(received);	

			if (CheckConnect(receivedMessage))
			{
				std::cout << "Connected to server successfully." << std::endl;
				isConnected = true;
			}

			else
			{
				std::cerr << "Failed to verify connection. Retrying..." << std::endl;
			}
		}

		else
		{
			std::cerr << "Failed to receive message from server. Retrying..." << std::endl;
		}

		if (!isConnected)
		{
			closesocket(sock);
			std::this_thread::sleep_for(std::chrono::seconds(1));
		}
	}

	closesocket(sock);
	WSACleanup();
}

bool TCPClient::IsConnected() const
{
	return false;
}

std::string_view TCPClient::GetCheckConnectMessage() const
{
	return "Hello, Server!";
}

bool TCPClient::CheckConnect(std::string_view message) const
{
	if (message.empty())
		return false;

	return message == "Hello, Client!";
}