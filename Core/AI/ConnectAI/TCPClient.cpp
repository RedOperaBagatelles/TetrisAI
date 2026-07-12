#include "TCPClient.h"

#pragma warning(disable:4996)

#include <iostream>
#include <winsock2.h>

#pragma comment(lib, "ws2_32.lib")

TCPClient::TCPClient()
{
	// Windows에서 소캣을 사용하기 위해 WinSock 라이브러리를 초기화
	WSADATA wsa;
	WSAStartup(MAKEWORD(2, 2), &wsa);							// WinSock 2.2 버전으로 초기화

	SOCKET sock = socket(AF_INET, SOCK_STREAM, 0);				// TCP 소켓 생성 (AF_INET : IPv4, SOCK_STREAM : TCP [SOCK_DGRAM : UDP])

	// 서버 정보를 저장하는 구조체 초기화
	sockaddr_in server;
	server.sin_family = AF_INET;
	server.sin_port = htons(5000);								// htos : 네트워크 표준인 빅엔디안 순서로 변경
	server.sin_addr.s_addr = inet_addr("127.0.0.1");

	// 서버 접속
	if (connect(sock, (sockaddr*)&server, sizeof(server)) == SOCKET_ERROR)
		std::cerr << "Failed to connect to server." << std::endl;
	
	// 데이터 전송
	const char* state = "Hello, Server!";
	send(sock, state, strlen(state), 0);

	// 서버로부터 응답 받음
	char buffer[1024];
	int received = recv(sock, buffer, sizeof(buffer) - 1, 0);	// 반환 값 : 수신된 바이트 수

	if (received > 0)
	{
		buffer[received] = '\0';
		std::cout << "AI : " << buffer << std::endl;
	}

	closesocket(sock);
	WSACleanup();
}
