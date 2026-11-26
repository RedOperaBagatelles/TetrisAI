#include "TCPClient.h"
#include "Utility/Json.h"
#include "Core/Tetris.h"
#include "Core/PiecesQueue.h"
#include "Core/AI/AIOutput.h"
#include "Core/AI/AIInput.h"

#include <cctype>
#include <iostream>
#include <random>
#include <string>
#include <string_view>
#include <thread>
#include <chrono>
#include <vector>
#include <memory>

#pragma comment(lib, "ws2_32.lib")
#pragma warning(disable:4996)

TCPClient& TCPClient::GetInstance()
{
	static TCPClient instance;

	return instance;
}

TCPClient::TCPClient() : sessionID(GenerateSessionID())
{
}

SessionID TCPClient::GenerateSessionID()
{
	// 같은 머신에서 클라이언트를 다시 실행해도 gameID(1부터의 순번)가 겹치지 않도록 세션마다 새 ID를 만든다.
	std::random_device randomDevice;
	const SessionID high = static_cast<SessionID>(randomDevice());
	const SessionID low = static_cast<SessionID>(randomDevice());

	return (high << 32) | low;
}

bool TCPClient::ExtractNumberField(std::string_view message, std::string_view key, std::uint64_t& value)
{
	const std::string searchKey = "\"" + std::string(key) + "\"";
	const std::size_t keyPos = message.find(searchKey);

	if (keyPos == std::string_view::npos)
		return false;

	const std::size_t colonPos = message.find(':', keyPos + searchKey.size());

	if (colonPos == std::string_view::npos)
		return false;

	std::size_t pos = colonPos + 1;

	while (pos < message.size() && std::isspace(static_cast<unsigned char>(message[pos])))
		pos++;

	const bool isQuoted = pos < message.size() && message[pos] == '"';

	if (isQuoted)
		pos++;

	const std::size_t tokenStart = pos;
	std::size_t tokenEnd = pos;

	if (isQuoted)
	{
		while (tokenEnd < message.size() && message[tokenEnd] != '"')
			tokenEnd++;
	}

	else
	{
		while (tokenEnd < message.size() && std::isdigit(static_cast<unsigned char>(message[tokenEnd])))
			tokenEnd++;
	}

	if (tokenEnd == tokenStart)
		return false;

	try
	{
		value = std::stoull(std::string(message.substr(tokenStart, tokenEnd - tokenStart)));
		return true;
	}

	catch (...)
	{
		return false;
	}
}

void TCPClient::SetGames(const std::vector<std::reference_wrapper<Tetris>>& newGames)
{
	games.clear();
	gameCount = newGames.size();

	// 새로운 게임 목록을 등록하고 gameID → Tetris* 매핑을 구성
	for (Tetris& game : newGames)
		games.emplace(game.GetGameID(), &game);
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
	root->AddChild(std::make_unique<Json>("GameCount", std::to_string(gameCount)));
	root->AddChild(std::make_unique<Json>("SessionID", std::to_string(sessionID)));
	root->AddChild(std::make_unique<Json>("GameIDSize", std::to_string(gameIDSize)));

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
					// 어떤 세션의 어떤 게임에 대한 명령인지 확인
					std::uint64_t commandSessionID = 0;
					std::uint64_t targetGameID = 0;

					if (!ExtractNumberField(message, "SessionID", commandSessionID) ||
						!ExtractNumberField(message, "GameID", targetGameID))
					{
						std::cerr << "SelectPathIndex response is missing SessionID/GameID. Ignored.\n";
						continue;
					}

					// Actions 배열 추출
					size_t actionsPos = message.find("\"Actions\"");

					if (actionsPos != std::string::npos)
					{
						size_t arrStart = message.find('[', actionsPos);
						size_t arrEnd = message.find(']', actionsPos);

						if (arrStart != std::string::npos && arrEnd != std::string::npos && arrEnd > arrStart)
						{
							std::string actionsStr = message.substr(arrStart + 1, arrEnd - arrStart - 1);
							std::vector<std::string> actionNames;
							std::string current;

							bool inQuotes = false;

							for (char c : actionsStr)
							{
								if (c == '"')
								{
									inQuotes = !inQuotes;

									if (!inQuotes && !current.empty())
									{
										actionNames.push_back(current);
										current.clear();
									}

									else if (inQuotes)
										current.clear();

									continue;
								}

								if (inQuotes)
									current += c;
							}

							if (!actionNames.empty())
								ExecuteActions(static_cast<SessionID>(commandSessionID), static_cast<GameCapacity>(targetGameID), actionNames);
						}
					}
				}
			}

			catch (...)
			{
				// 파싱 실패시 무시
			}

			continue;
		}
	}
}

void TCPClient::ExecuteActions(SessionID commandSessionID, GameCapacity targetGameID, const std::vector<std::string>& actionNames)
{
	// 이전 세션에서 온 명령은 무시 (재접속 후 서버가 오래된 게임을 조작하지 않도록)
	if (commandSessionID != sessionID)
	{
		std::cerr << "Ignoring command from another session. expected=" << sessionID << ", received=" << commandSessionID << "\n";
		return;
	}

	const auto it = games.find(targetGameID);

	if (it == games.end())
	{
		std::cerr << "Ignoring command for unknown gameID=" << targetGameID << "\n";
		return;
	}

	Tetris& target = *it->second;

	// 이미 Game Over가 된 게임은 서버가 보낸 액션을 실행하지 않는다.
	if (target.IsGameOver())
		return;

	AIInput aiInput(target);

	// 액션 시퀀스를 실행하는 동안에는 조각 생성 시점의 상태 전송을 보류
	isExecutingActions = true;

	for (const auto& name : actionNames)
	{
		if (name == "MOVE_LEFT")					aiInput.Action(AIAction::MoveLeft);
		else if (name == "MOVE_RIGHT")				aiInput.Action(AIAction::MoveRight);
		else if (name == "MOVE_DOWN")				aiInput.Action(AIAction::MoveDown);
		else if (name == "ROTATE_CLOCKWISE")		aiInput.Action(AIAction::RotateClockwise);
		else if (name == "ROTATE_COUNTERCLOCKWISE")	aiInput.Action(AIAction::RotateCounterClockwise);
		else if (name == "HARD_DROP")				aiInput.Action(AIAction::HardDrop);
		else if (name == "HOLDING")					aiInput.Action(AIAction::Holding);
		else if (name == "NONE")					aiInput.Action(AIAction::None);
	}

	isExecutingActions = false;

	// 배치가 모두 완료된 후 해당 보드의 상태와 다음 조각의 가능한 배치를 서버로 전송
	target.SendStateAndPlacements();
}
