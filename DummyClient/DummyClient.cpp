#include "pch.h"
#include <iostream>

#include <WinSock2.h>
#include <MSWSock.h>
#include <WS2tcpip.h>
#pragma comment(lib, "ws2_32.lib")

void HandleError(const char* cause)
{
	int32 errCode = ::WSAGetLastError();
	cout << cause << " ErrorCode : " << errCode << endl;
}

int main()
{
	this_thread::sleep_for(1s);

	WSAData wsaData;
	if (::WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
		return 0;

	SOCKET clientSocket = ::socket(AF_INET, SOCK_STREAM, 0);
	if (clientSocket == INVALID_SOCKET)
	{
		HandleError("Socket");
		return 0;
	}

	u_long on = 1;
	// 논블로킹 소켓으로 변환
	if (::ioctlsocket(clientSocket, FIONBIO, &on) == INVALID_SOCKET)
		return 0;

	SOCKADDR_IN serverAddr;
	::memset(&serverAddr, 0, sizeof(serverAddr));
	serverAddr.sin_family = AF_INET;
	::inet_pton(AF_INET, "127.0.0.1", &serverAddr.sin_addr);
	serverAddr.sin_port = ::htons(7777);

	// Connect
	while (true)
	{
		if (::connect(clientSocket, (SOCKADDR*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR)
		{
			if (::WSAGetLastError() == WSAEWOULDBLOCK)
				continue;
			// 이미 연결된 상태
			if (::WSAGetLastError() == WSAEISCONN)
				break;

			//ERORR
			break;
		}
	}

	cout << "Connected to Server!" << endl;

	char sendBuffer[100] = "Hello World";

	// Send
	while (true)
	{
		if (::send(clientSocket, sendBuffer, sizeof(sendBuffer), 0) == SOCKET_ERROR)
		{
			// 논블록킹이 일어날만한 상황
			if (::WSAGetLastError() == WSAEWOULDBLOCK)
				continue;

			//Error
			break;
		}

		cout << "Send Data ! Len = " << sizeof(sendBuffer) << endl;
		
		// Recv
		while (true)
		{
			char recvBuffer[1000];
			int32 recvLen = ::recv(clientSocket, recvBuffer, sizeof(recvBuffer), 0);
			if (recvLen == SOCKET_ERROR)
			{
				// 논블록킹이 일어날만한 상황
				if (::WSAGetLastError() == WSAEWOULDBLOCK)
					continue;

				//Error
				break;
			}
			else if (recvLen == 0)
			{
				// 연결 끊김
				break;
			}

			cout << "Recv Data Len = " << recvLen << endl;
			break;
		}

		this_thread::sleep_for(1s);
	}

	// 소켓 리소스 반환
	::closesocket(clientSocket);

	// 원속 종료
	::WSACleanup();
}

// socket option
/*
int main()
{
	WSAData wsaData;
	if (::WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
		return 0;

	SOCKET clientSocket = ::socket(AF_INET, SOCK_DGRAM, 0);
	if (clientSocket == INVALID_SOCKET)
	{
		HandleError("Socket");
		return 0;
	}



	// 소켓 리소스 반환
	::closesocket(clientSocket);

	// 원속 종료
	::WSACleanup();
}
*/
// UDP
/*
int main()
{
	WSAData wsaData;
	if (::WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
		return 0;

	SOCKET clientSocket = ::socket(AF_INET, SOCK_DGRAM, 0);
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
	::inet_pton(AF_INET, "127.0.0.1", &serverAddr.sin_addr);
	serverAddr.sin_port = ::htons(7777);

	// connected UDP
	// 소켓 자체에 즐겨찾기 처럼 대상을 정해줌
	// sendTo, recvFrom 으로 serverAddr 인자를 넘기지 않아도 됨
	::connect(clientSocket, (SOCKADDR*)&serverAddr, sizeof(serverAddr));
	

	while (true)
	{
		// TODO

		for (int32 i = 0; i < 10; i++)
		{
			char sendBuffer[100] = "Hello World!";

			// Unconnected UDP
			//int32 resultCode = ::sendto(clientSocket, sendBuffer, sizeof(sendBuffer), 0,
			//	(SOCKADDR*)&serverAddr, sizeof(serverAddr));

			// Connected UDP
			int32 resultCode = ::send(clientSocket, sendBuffer, sizeof(sendBuffer), 0);

			if (resultCode == SOCKET_ERROR)
			{
				HandleError("SendTo)");
				return 0;
			}

			cout << "Send Data! Len = " << sizeof(sendBuffer) << endl;
		}


		SOCKADDR_IN recvAddr;
		::memset(&recvAddr, 0, sizeof(recvAddr));
		int32 addrLen = sizeof(recvAddr);

		char recvBuffer[1000];

		// 패킷을 보낸 client의 주소와 버퍼를 받아옴
		// Unconnected UDP
		//int32 recvLen = ::recvfrom(clientSocket, recvBuffer, sizeof(recvBuffer), 0,
		//	(SOCKADDR*)&recvAddr, &addrLen);

		// Connected UDP
		int32 recvLen = ::recv(clientSocket, recvBuffer, sizeof(recvBuffer), 0);

		if (recvLen <= 0)
		{
			HandleError("RecvFrom");
			return 0;
		}

		cout << "Recv Data! Data = " << recvBuffer << endl;
		cout << "Recv Data! Len = " << recvLen << endl;

		this_thread::sleep_for(1s);
	} 

	// ------------------------

	// 소켓 리소스 반환
	::closesocket(clientSocket);

	// 원속 종료
	::WSACleanup();
}
*/
//TCP
/*
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
		char sendBuffer[100] = "Hello World!";

		for (int32 i = 0; i < 10; i++)
		{
			// 소켓에 전송할 버퍼와 크기를 송신
			int32 resultCode = ::send(clientSocket, sendBuffer, sizeof(sendBuffer), 0);
			if (resultCode == SOCKET_ERROR)
			{
				int32 errCode = ::WSAGetLastError();
				cout << "Send ErrorCode : " << errCode << endl;
				return 0;
			}
		}


		cout << "Send Data! Len = " << sizeof(sendBuffer) << endl;

		//char recvBuffer[1000];

		//int32 recvLen = ::recv(clientSocket, recvBuffer, sizeof(recvBuffer), 0);
		//if (recvLen <= 0)
		//{
		//	int32 errCode = ::WSAGetLastError();
		//	cout << "Recv ErrorCode : " << errCode << endl;
		//	return 0;
		//}

		//cout << "Recv Data! Data = " << recvBuffer << endl;
		//cout << "Recv Data! Len = " << recvLen << endl;

		this_thread::sleep_for(1s);
	}

	// ------------------------

	// 소켓 리소스 반환
	::closesocket(clientSocket);

	// 원속 종료
	::WSACleanup();
}
*/