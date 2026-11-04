#include "TCPClient.h"
#include "Utility/Json.h"
#include "Core/Tetris.h"
#include "Core/PiecesQueue.h"
#include "Core/AI/AIOutput.h"

#include <iostream>
#include <string>
#include <thread>
#include <chrono>
#include <vector>
#include <sstream>

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
		std::string message = GetCheckConnectMessage();
		send(sock, message.data(), static_cast<int>(message.size()), 0);

		// 서버로부터 응답 받음
		std::string receivedMessage(1024, '\0');
		int received = recv(sock, receivedMessage.data(), static_cast<int>(receivedMessage.size()), 0);	// 반환 값 : 수신된 바이트 수

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
				std::cerr << "Failed to verify connection. Received: " << receivedMessage << " / Retrying..." << std::endl;
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

	StartReceiveThread();
}

TCPClient::~TCPClient()
{
	StopReceiveThread();

	if (sock != INVALID_SOCKET)
	{
		closesocket(sock);

		sock = INVALID_SOCKET;
	}

	WSACleanup();
}

bool TCPClient::IsConnected() const
{
	return isConnected;
}

void TCPClient::SendMessage(const std::string& message) const
{
	if (sock == INVALID_SOCKET)
	{
		std::cerr << "Socket is not valid. Cannot send message.\n";
		return;
	}

	int sent = send(sock, message.data(), static_cast<int>(message.size()), 0);

	if (sent == SOCKET_ERROR)
		std::cerr << "Failed to send message.\n";
}

void TCPClient::SendMessage(const std::vector<uint8_t>& bytes) const
{
	if (sock == INVALID_SOCKET)
	{
		std::cerr << "Socket is not valid. Cannot send message.\n";
		return;
	}

	int sent = send(sock, reinterpret_cast<const char*>(bytes.data()), static_cast<int>(bytes.size()), 0);

	if (sent == SOCKET_ERROR)
		std::cerr << "Failed to send game state message.\n";
}

std::string TCPClient::GetCheckConnectMessage() const
{
	auto root = std::make_unique<Json>();
	root->AddChild(std::make_unique<Json>("Type", "ConnectionCheck"));
	root->AddChild(std::make_unique<Json>("Status", "Request"));

	// GameState.py 와 동기화 검증을 위해 크기 정보를 함께 전송 (보드 크기, 큐 크기, 메시지 크기)
	root->AddChild(std::make_unique<Json>("BoardWidth", std::to_string(Tetris::width)));
	root->AddChild(std::make_unique<Json>("BoardHeight", std::to_string(Tetris::maxHeight)));
	root->AddChild(std::make_unique<Json>("QueueSize", std::to_string(PiecesQueue::queueSize)));
	root->AddChild(std::make_unique<Json>("MessageSize", std::to_string(AIOutput::messageSize)));
	root->AddChild(std::make_unique<Json>("PieceTypeCount", std::to_string(static_cast<unsigned int>(PieceType::Count))));

	return Json::ToString(root.get());
}

bool TCPClient::CheckConnect(std::string_view message) const
{
	if (message.empty())
		return false;

	auto expectedRoot = std::make_unique<Json>();
	expectedRoot->AddChild(std::make_unique<Json>("Type", "ConnectionCheck"));
	expectedRoot->AddChild(std::make_unique<Json>("Status", "Response"));

	std::string expectedStr = Json::ToString(expectedRoot.get());

	return message == expectedStr;
}

void TCPClient::StartReceiveThread()
{
	// 이미 수신 스레드가 실행 중이면 종료
	if (isReceiving)
		return;

	isReceiving = true;
	receiveThread = std::thread(&TCPClient::ReceiveLoop, this);
}

void TCPClient::StopReceiveThread()
{
	// 이미 수신 스레드가 중지되었거나 소켓이 유효하지 않으면 종료
	if (!isReceiving)
		return;

	isReceiving = false;

	if (sock != INVALID_SOCKET)
		shutdown(sock, SD_BOTH);

	// 수신 스레드가 종료될 때까지 대기
	if (receiveThread.joinable())
		receiveThread.join();
}

void TCPClient::ReceiveLoop()
{
	while (isReceiving && sock != INVALID_SOCKET)
	{
		char buffer[4096];
		int received = recv(sock, buffer, static_cast<int>(sizeof(buffer) - 1), 0);

		if (received <= 0)
		{
			if (isReceiving)
				std::cerr << "Connection lost or failed to receive from server.\n";
			break;
		}

		buffer[received] = '\0';
		std::string message(buffer, received);

		// 서버로부터 수신한 응답을 콘솔에 출력
		std::cout << "[Server Response] " << message << std::endl;

		// SelectPathIndex 응답 처리
		if (message.find("\"Type\":\"SelectPathIndex\"") != std::string::npos || message.find("\"Type\": \"SelectPathIndex\"") != std::string::npos)
		{
			try
			{
				// 간단한 파싱으로 필요한 정보 추출
				size_t statusPos = message.find("\"Status\"");
				if (statusPos == std::string::npos)
					continue;

				size_t colonPos = message.find(':', statusPos);
				if (colonPos == std::string::npos)
					continue;

				size_t quote1 = message.find('"', colonPos + 1);
				if (quote1 == std::string::npos)
					continue;
				size_t quote2 = message.find('"', quote1 + 1);
				if (quote2 == std::string::npos)
					continue;
				std::string status = message.substr(quote1 + 1, quote2 - quote1 - 1);

				if (status == "Response")
				{
					// Index 추출
					auto ExtractInt = [&](const std::string& key) -> int {
						size_t keyPos = message.find("\"" + key + "\"");
						if (keyPos == std::string::npos) return -1;
						size_t c = message.find(':', keyPos);
						if (c == std::string::npos) return -1;
						size_t numStart = message.find_first_of("-0123456789", c);
						if (numStart == std::string::npos) return -1;
						size_t numEnd = message.find_first_not_of("-0123456789", numStart);
						std::string numStr = (numEnd == std::string::npos) ? message.substr(numStart) : message.substr(numStart, numEnd - numStart);
						try { return std::stoi(numStr); } catch (...) { return -1; }
					};

					int index = ExtractInt("Index");
					int x = ExtractInt("X");
					int y = ExtractInt("Y");
					int rotation = ExtractInt("Rotation");
					int actionCount = ExtractInt("ActionCount");

					std::cout << "[Selected Path] Index=" << index << " (X=" << x << ", Y=" << y << ", Rotation=" << rotation << ") | Actions=" << actionCount << std::endl;

					// Actions 배열 추출
					size_t actionsPos = message.find("\"Actions\"");
					if (actionsPos != std::string::npos)
					{
						size_t arrStart = message.find('[', actionsPos);
						size_t arrEnd = message.find(']', actionsPos);
						if (arrStart != std::string::npos && arrEnd != std::string::npos && arrEnd > arrStart)
						{
							std::string actionsStr = message.substr(arrStart + 1, arrEnd - arrStart - 1);
							std::cout << "[Selected Path] ActionSequence: " << actionsStr << std::endl;
						}
					}
				}
				else if (status == "Error")
				{
					std::cout << "[Selected Path] Error response received." << std::endl;
				}
			}
			catch (...)
			{
				// 파싱 실패시 무시
			}
		}
	}
}