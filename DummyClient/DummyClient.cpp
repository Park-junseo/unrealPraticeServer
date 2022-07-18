#include "pch.h"
#include <iostream>

#include <WinSock2.h>
#include <MSWSock.h>
#include <WS2tcpip.h>
#pragma comment(lib, "ws2_32.lib")

int main()
{
	// 원속 초기화 (ws2_32 라이브러리 초기화)
	// 관련 정보가 wsaData에 채워짐
	WSAData wsaData;
	if (::WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
		return 0;

	// ad : Address Family (AF_INET = IPv4, AF_INET6 = IPv6)
	// type : TCP(SOCK_STREAM) vs UDP(SOCK_DGRAM)
	// protocol : 0
	// return : descriptor
	// SOKET은 socket 함수로 할당된 소켓 리소스 번호
	SOCKET clientSocket = ::socket(AF_INET, SOCK_STREAM, 0);
	if (clientSocket == INVALID_SOCKET)
	{
		int32 errCode = ::WSAGetLastError(); // 어떤 이유로 안되는지 에러 코드를 받아옴
		cout << "Soket ErrorCode : " << errCode << endl;
		return 0;
	}

	// 연결할 목적지: IP주소 + Port
	SOCKADDR_IN serverAddr; // IPv4 구조체
	::memset(&serverAddr, 0, sizeof(serverAddr)); // 구조체 초기화
	serverAddr.sin_family = AF_INET;
	//serverAddr.sin_addr.s_addr = ::inet_addr("127.0.0.1");
	::inet_pton(AF_INET, "127.0.0.1", &serverAddr.sin_addr); // 서버 주소가 자기 자신이므로
	serverAddr.sin_port = ::htons(7777); //포트의 임의 번호 지정/ 일부포트는 예약되어 있음

	// host to newwork short
	// Little-Endian vs Big=Endian
	// 현재환경에서 little endian 방식으로 메모리를 관리해도 다른 클라이언트가 다를 수 있음

	// 소켓 연결
	if (::connect(clientSocket, (SOCKADDR*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR)
	{
		//소켓 연결 안될 시
		int32 errCode = ::WSAGetLastError();
		cout << "Connect ErrorCode : " << errCode << endl;
		return 0;
	}


	// ------------------------
	// 연결 성공! 이제부터 데이터 송수신 가능!

	cout << "Connected To Server!" << endl;

	while (true)
	{
		// TODO

		this_thread::sleep_for(1s);
	}

	// ------------------------

	// 소켓 리소스 반환
	::closesocket(clientSocket);

	// 원속 종료
	::WSACleanup();
}