#include "pch.h"
#include "ThreadManager.h"

#include "Service.h"
#include "Session.h"
#include "GameSession.h"
#include "GameSessionManager.h"
#include "BufferWriter.h"
#include "ClientPacketHandler.h"
#include "Protocol.pb.h"

int main()
{
	ClientPacketHandler::Init();

	ServerServiceRef service = MakeShared<ServerService>(
		NetAddress(L"127.0.0.1", 7777),
		MakeShared<IocpCore>(),
		MakeShared<GameSession>, // TODO : SessionManager 등
		100);

	ASSERT_CRASH(service->Start());

	for (int32 i = 0; i < 5; i++)
	{
		GThreadManager->Launch([=]()
			{
				while (true)
				{
					service->GetIocpCore()->Dispatch();
				}
			});
	}

	//char sendData[1000] = "가"; // CP949 = KS-X-1001(한글2바이트) + KS-X-1003(로마 1바이트)
	WCHAR sendData3[1000] = L"가"; // UTF16 = Unicode (한글/로마 2바이트)

	while (true)
	{
		Protocol::S_TEST pkt;
		pkt.set_id(1000);
		pkt.set_hp(100);
		pkt.set_attack(10);
		{
			Protocol::BuffData* data = pkt.add_buffs();
			data->set_buffid(100);
			data->set_remaintime(1.2f);
			data->add_victims(4000);
		}
		{
			Protocol::BuffData* data = pkt.add_buffs();
			data->set_buffid(200);
			data->set_remaintime(2.5f);
			data->add_victims(1000);
			data->add_victims(2000);
		}

		SendBufferRef sendBuffer = ClientPacketHandler::MakeSendBuffer(pkt);

		GSessionManager.Broadcast(sendBuffer);

		this_thread::sleep_for(250ms);
	}

	GThreadManager->Join();
}

// Packet Serialization
/*
#include <tchar.h>

int main()
{
	ServerServiceRef service = MakeShared<ServerService>(
		NetAddress(L"127.0.0.1", 7777),
		MakeShared<IocpCore>(),
		MakeShared<GameSession>, // TODO : SessionManager 등
		100);

	ASSERT_CRASH(service->Start());

	for (int32 i = 0; i < 5; i++)
	{
		GThreadManager->Launch([=]()
			{
				while (true)
				{
					service->GetIocpCore()->Dispatch();
				}
			});
	}

	//char sendData[1000] = "가"; // CP949 = KS-X-1001(한글2바이트) + KS-X-1003(로마 1바이트)
	//char sendData2[1000] = u8"가"; // UTF8 = Unicode (한글3바이트 + 로마1바이트)
	WCHAR sendData3[1000] = L"가"; // UTF16 = Unicode (한글/로마 2바이트)
	//TCHAR sendData4[1000] = _T("가");

	//while (true)
	//{
	//	SendBufferRef sendBuffer = GSendBufferManager->Open(4096);
	//	
	//	BYTE* buffer = sendBuffer->Buffer();
	//	((PacketHeader*)buffer)->size = (sizeof(sendData) + sizeof(PacketHeader));
	//	((PacketHeader*)buffer)->id = 1; // 1: Hello Msg
	//	
	//	::memcpy(&buffer[4], sendData,sizeof(sendData));
	//	sendBuffer->Close(sizeof(sendData) + sizeof(PacketHeader));

	//	//Send(sendBuffer);
	//	// BroadCast로 변경
	//	GSessionManager.Broadcast(sendBuffer);
	//	
	//	this_thread::sleep_for(250ms);
	//}

	while (true)
	{
		PKT_S_TEST_WRITE pktWriter(1001, 100, 10);

		PKT_S_TEST_WRITE::BuffsList buffList =  pktWriter.ReserveBuffsList(3);
		buffList[0] = { 100, 1.5f };
		buffList[1] = { 200, 2.3f };
		buffList[2] = { 300, 0.7f };

		PKT_S_TEST_WRITE::BuffsVictimsList vic0 = pktWriter.RreserveBuffsVictimList(&buffList[0], 3);
		{
			vic0[0] = 1000;
			vic0[1] = 2000;
			vic0[2] = 3000;
		}

		PKT_S_TEST_WRITE::BuffsVictimsList vic1 = pktWriter.RreserveBuffsVictimList(&buffList[1], 1);
		{
			vic1[0] = 1000;
		}

		PKT_S_TEST_WRITE::BuffsVictimsList vic2 = pktWriter.RreserveBuffsVictimList(&buffList[2], 2);
		{
			vic2[0] = 3000;
			vic2[1] = 5000;
		}

		SendBufferRef sendBuffer = pktWriter.CloseAndReturn();

		GSessionManager.Broadcast(sendBuffer);

		this_thread::sleep_for(250ms);
	}

	GThreadManager->Join();
}
*/
// ...SendBuffer Pooling
/*
//class GameSession2 : public Session
//{
//public:
//	~GameSession2()
//	{
//		cout << "~GameSession" << endl;
//	}
//
//	virtual int32 OnRecv(BYTE* buffer, int32 len) override
//	{
//		//// Echo
//		//cout << "OnRecv Len = " << len << endl;
//		//Send(buffer, len);
//		//return len;
//
//		// Echo
//		SendBufferRef sendBuffer = MakeShared<SendBuffer>(4096);
//		sendBuffer->CopyData(buffer, len);
//		Send(sendBuffer);
//		return len;
//	}
//
//	virtual void OnSend(int32 len) override
//	{
//		cout << "OnSend Len = " << len << endl;
//	}
//};

int main()
{
	ServerServiceRef service = MakeShared<ServerService>(
		NetAddress(L"127.0.0.1", 7777),
		MakeShared<IocpCore>(),
		MakeShared<GameSession>, // TODO : SessionManager 등
		100);

	ASSERT_CRASH(service->Start());

	for (int32 i = 0; i < 5; i++)
	{
		GThreadManager->Launch([=]()
			{
				while (true)
				{
					service->GetIocpCore()->Dispatch();
				}
			});
	}

	GThreadManager->Join();
}
*/
// ... Session #2
/*
#include "CorePch.h"
#include <thread>
#include <atomic>
#include <mutex>

#include <Windows.h>

#include <future>

//#include "SocketUtils.h"
//#include "Listener.h"

class GameSession : public Session
{
public:
	virtual int32 OnRecv(BYTE* buffer, int32 len) override
	{
		// Echo
		cout << "OnRecv Len = " << len << endl;
		Send(buffer, len);
		return len;
	}

	virtual void OnSend(int32 len) override
	{
		cout << "OnSend Len = " << len << endl;
	}
};

int main()
{
	//ListenerRef listener = MakeShared<Listener>();
	//listener->StartAccept(NetAddress(L"127.0.0.1", 7777));

	//for (int32 i = 0; i < 5; i++)
	//{
	//	GThreadManager->Launch([=]()
	//		{
	//			while (true)
	//			{
	//				GIocpCore.Dispatch();
	//			}
	//		});
	//}

	ServerServiceRef service = MakeShared<ServerService>(
		NetAddress(L"127.0.0.1", 7777),
		MakeShared<IocpCore>(),
		MakeShared<GameSession>, // TODO : SessionManager 등
		100);

	ASSERT_CRASH(service->Start());

	for (int32 i = 0; i < 5; i++)
	{
		GThreadManager->Launch([=]()
			{
				while (true)
				{
					service->GetIocpCore()->Dispatch();
				}
			});
	}

	GThreadManager->Join();
}
*/
// IocpCore
/*
int main()
{
	//SOCKET socket = SocketUtils::CreateSocket();

	//SocketUtils::BindAnyAddress(socket, 7777);

	//SocketUtils::Listen(socket);

	//SOCKET clientSocket = ::accept(socket, nullptr, nullptr);

	// IOCP를 관찰하는 쓰레드를 만들어 줘야 함
	Listener listener;
	listener.StartAccept(NetAddress(L"127.0.0.1", 7777));

	//while (true)
	//{

	//}

	for (int32 i = 0; i < 5; i++)
	{
		GThreadManager->Launch([=]()
			{
				while (true)
				{
					GIocpCore.Dispatch();
				}
			});
	}

	GThreadManager->Join();
}
*/
//Completion Port 모델
/*
#include <WinSock2.h>
#include <MSWSock.h>
#include <WS2tcpip.h>
#pragma comment(lib, "ws2_32.lib")

#include "Memory.h"

void HandleError(const char* cause)
{
	int32 errCode = ::WSAGetLastError();
	cout << cause << " ErrorCode : " << errCode << endl;
}

const int32 BUFSIZE = 1000;
// 클라이언트가 접속할 떄 Session 구조체로 정보를 관리
struct Session
{
	SOCKET socket = INVALID_SOCKET;
	char recvBuffer[BUFSIZE];
	int32 recvBytes = 0;
	int32 sendBytes = 0;
};

enum IO_TYPE
{
	READ,
	WRITE,
	ACCEPT,
	CONNECT
};

struct OverlappedEx
{
	WSAOVERLAPPED overlapped = {};
	int32 type = 0; // read, write, accpet, connect ..
};

void CALLBACK RecvCallback(DWORD error, DWORD recvLen, LPWSAOVERLAPPED overlapped, DWORD flags)
{
	cout << "Data Recv Len Cal'lback = " << recvLen << endl;
	// TODO : 에코 서버를 만든다면 WSASend()

	Session* session = (Session*)overlapped;
}

void WorkerThreadMain(HANDLE iocpHandle)
{
	while (true)
	{
		//소켓에 송수신된 바이트 수를 넘겨주기 위함
		DWORD bytesTransferred = 0;
		Session* session = nullptr;
		OverlappedEx* overlappedEx = nullptr;
		BOOL ret = ::GetQueuedCompletionStatus(iocpHandle, &bytesTransferred,
			(ULONG_PTR*)&session, (LPOVERLAPPED*)&overlappedEx, INFINITE);

		if (ret == FALSE || bytesTransferred == 0)
		{
			// TODO : 연결 끊김
			continue;
		}

		ASSERT_CRASH(overlappedEx->type == IO_TYPE::READ);

		cout << "Recv Data IOCP = " << bytesTransferred << endl;

		WSABUF wsaBuf;
		wsaBuf.buf = session->recvBuffer;
		wsaBuf.len = BUFSIZE;

		// 클라이언트와 1대1 대응이 아니므로 스택메모리가 아닌 동적할당으로 넘겨줄 것
		//OverlappedEx* overlappedEx = new OverlappedEx();
		//overlappedEx->type = IO_TYPE::READ;

		DWORD recvLen = 0;
		DWORD flags = 0;
		::WSARecv(session->socket, &wsaBuf, 1, &recvLen, &flags, &overlappedEx->overlapped, NULL);
	}
}

int main()
{
	WSAData wsaData;
	if (::WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
		return 0;

	SOCKET listenSocket = ::socket(AF_INET, SOCK_STREAM, 0);
	if (listenSocket == INVALID_SOCKET)
		return 0;

	SOCKADDR_IN serverAddr;
	::memset(&serverAddr, 0, sizeof(serverAddr));
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_addr.s_addr = ::htonl(INADDR_ANY);
	serverAddr.sin_port = ::htons(7777);

	if (::bind(listenSocket, (SOCKADDR*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR)
		return 0;

	if (::listen(listenSocket, SOMAXCONN) == SOCKET_ERROR)
		return 0;

	cout << "Accept" << endl;

	// Overlapped 모델 (Completion Routine 콜백 기반)
	// - 비동기 입출력 함수 완료되면, 쓰레드마다 있는 APC 큐에 일감이 쌓임
	// - Alertable Wait 상태로 들어가서 APC 큐 비우기 (콜백 함수)
	// 단점) APC 큐 쓰레드마다 있다! Alertable Wait 자체도 조금 부담!
	// 단점) 이벤트 방식 소켓: 이벤트 1:1 대응

	// IOCP (Completion Port) 모델
	// - APC -> Completion Port (쓰레드마다 있는 건 아니고 1개. 중앙에서 관리하는 APC 큐?)
	// - Alertable Wait -> CP 결과 처리를 GetQueuedCompletionStatus
	// 멀티 쓰레드 환경에서 사용하는 것이 일반적, 쓰레드와 궁합이 굉장히 좋음

	// CreateIoCompletionPort
	// GetQueuedCompletionStatus

	// 모든 섹션을 들고있는 상태
	vector<Session*> sessionManager;	

	// CP 생성 또는 기존의 소켓을 IOCP에 등록할 떄
	// CP 생성일 때는 첫 인자에 INVALID_HANDLE_VALUE
	HANDLE iocpHandle = ::CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, 0);

	// WorkerThreads
	for (int32 i = 0; i < 5; i++)
		GThreadManager->Launch([=]() {WorkerThreadMain(iocpHandle); });

	// 메인 쓰레드: accpet 담당
	while (true)
	{
		SOCKADDR_IN clientAddr;
		int32 addrLen = sizeof(clientAddr);

		SOCKET clientSocket = accept(listenSocket, (SOCKADDR*)&clientAddr, &addrLen);
		if (clientSocket == INVALID_SOCKET)
			return 0;

		//Session* session = new Session();
		Session* session = xnew<Session>();
		session->socket = clientSocket;
		sessionManager.push_back(session);

		cout << "Client Connected !" << endl;

		// 소켓을 CP에 등록 (관찰대상임을 알림)
		::CreateIoCompletionPort((HANDLE)clientSocket, iocpHandle, 
			(ULONG_PTR)session, // Key, 넘겨준 소켓에 대한 임의의 키값을 설정 가능
			0 //최대쓰레드, 0 일시 최대 코어 개수만큼 할당
		);

		WSABUF wsaBuf;
		wsaBuf.buf = session->recvBuffer;
		wsaBuf.len = BUFSIZE;

		// 클라이언트와 1대1 대응이 아니므로 스택메모리가 아닌 동적할당으로 넘겨줄 것
		OverlappedEx* overlappedEx = new OverlappedEx();
		overlappedEx->type = IO_TYPE::READ;

		DWORD recvLen = 0;
		DWORD flags = 0;
		::WSARecv(clientSocket, &wsaBuf, 1, &recvLen, &flags, &overlappedEx->overlapped, NULL);

		// 유저가 게임 접속 종료
		//Session* s = sessionManager.back();
		//sessionManager.pop_back();
		//// 하지만 임의로 삭제 시, 오염된 메모리에 접근할 수 있음. xnew 사용
		////delete s;
		//xdelete(s);
	}

	GThreadManager->Join();

	// 원속 종료
	::WSACleanup();
}
*/
// Overlapped 모델 (콜백 기반)
/*
struct Session
{
	// Overlapped Model (Base callback)
	// 구조체 변수를 함수의 매개변수로 넘길 때,
	// 넘겨야 하는 타입을 구조체 속성에 첫번째로 두면 캐스팅할 때 구조체의
	// 첫번째 타입으로 변환할 수 있고, 함수에서 넘겨받은 구조체 속성으로
	// 구조체로 캐스팅하여 사용할 수 있음
	WSAOVERLAPPED overlapped = {};

	SOCKET socket = INVALID_SOCKET;
	char recvBuffer[BUFSIZE];
	int32 recvBytes = 0;
	int32 sendBytes = 0;
	// Overlapped Model
	//WSAOVERLAPPED overlapped = {};
};

int main()
{
	WSAData wsaData;
	if (::WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
		return 0;

	SOCKET listenSocket = ::socket(AF_INET, SOCK_STREAM, 0);
	if (listenSocket == INVALID_SOCKET)
		return 0;

	u_long on = 1;
	if (::ioctlsocket(listenSocket, FIONBIO, &on) == INVALID_SOCKET)
		return 0;

	SOCKADDR_IN serverAddr;
	::memset(&serverAddr, 0, sizeof(serverAddr));
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_addr.s_addr = ::htonl(INADDR_ANY);
	serverAddr.sin_port = ::htons(7777);

	if (::bind(listenSocket, (SOCKADDR*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR)
		return 0;

	if (::listen(listenSocket, SOMAXCONN) == SOCKET_ERROR)
		return 0;

	cout << "Accept" << endl;

	// Overlapped 모델 (Completion Routine 콜백 기반)
	// - 비동기 입출력 지원하는 소켓 생성
	// - 비동기 입출력 함수 호출 (완료 루틴의 시작 주소를 넘겨준다)
	// - 비동기 작업이 바로 완료되지 않으면, WSA_IO_PENDING 오류 코드
	// - 비동기 입출력 함수 호출한 쓰레드를 -> Alertable Wait 상태로 만든다
	// Alertable Wait 상태 변환 함수: ex) WaitForSingleObjectEx, WaitForMultipleObjectsEx, SleepEx, WSAWaitForMultipleEvents
	// - Wait에서 비동기 IO 완료되면, 운영체제는 완료 루틴 호출
	// - 완료 루틴 호출이 모두 끝나면, 쓰레드는 Alertable Wait 상태에서 빠져나옴

	// 1) 오류 발생시 0 아닌 값
	// 2) 전송 바이트 수
	// 3) 비동기 입출력 함수 호출 시 넘겨준 WSAOVERLAPPED 구조체의 주소값
	// 4) 0
	//void CompletionRoutine()

	//Select 모델
	//장점 : 윈도우 / 리눅스 공통
	//단점 : 성능 최하(매번 등록 비용), 64개 제한
	//WSAEventSelect 모델
	//장점 : 비교적 뛰어난 성능
	//단점 : 64개 제한
	//Overlapped 모델(이벤트 기반)
	//장점 : 성능
	//단점 : 64개 제한
	//Overlapped 모델(콜백 기반)
	//장점 : 성능
	//단점 : 모든 비동기 소켓 함수에서 사용 가능하진 않음(accpet) 빈번한 Alertable Wait로 인한 성능 저하
	//IOCP 모델
	//라이브러리에 사용될 모델

	//Rreactor Pattern(-뒤늦게.논블로킹 소켓.소켓 상태 확인 후->뒤늦게 recv send 호출)
	//Proactor Pattern(-미리 Overlapped WSA - )

	while (true)
	{
		SOCKADDR_IN clientAddr;
		int32 addrLen = sizeof(clientAddr);

		SOCKET clientSocket;
		while (true)
		{
			// listenSocke이 논블록이므로 clientSocket도 논블록
			clientSocket = ::accept(listenSocket, (SOCKADDR*)&clientAddr, &addrLen);
			if (clientSocket != INVALID_SOCKET)
				break;

			if (::WSAGetLastError() == WSAEWOULDBLOCK)
				continue;

			//문제 있는 상황
			return 0;
		}

		Session session = Session{ clientSocket };
		WSAEVENT wsaEvent = ::WSACreateEvent();
		session.overlapped.hEvent = wsaEvent;

		cout << "Client Connected !" << endl;

		while (true)
		{
			WSABUF wsaBuf;
			wsaBuf.buf = session.recvBuffer;
			wsaBuf.len = BUFSIZE;

			DWORD recvLen = 0;
			DWORD flags = 0;
			//recv를 비동기 방식으로 호출
			if (::WSARecv(clientSocket, &wsaBuf, 1, &recvLen, &flags, &session.overlapped, RecvCallback) == SOCKET_ERROR)
			{
				if (::WSAGetLastError() == WSA_IO_PENDING)
				{
					// Pending
					//Alertable Wait
					// ::WSAWaitForMultipleEvents(1, &wsaEvent, TRUE, WSA_INFINITE, FALSE);
					// 마지막 인자가 true 면 alertable wait 상태가 됨
					//::WSAWaitForMultipleEvents(1, &wsaEvent, TRUE, WSA_INFINITE, TRUE); 

					// Alertalbe Wait 상태로 변환, 호출할 콜백 함수가 있는지 판별될 때까지 대기
					SleepEx(INFINITE, TRUE); // 모든 콜백들을 호출하므로 클라이언트 개수 따라 이벤트를 할당할 필요 없음


					// ::WSAGetOverlappedResult(session.socket, &session.overlapped, &recvLen, FALSE, &flags);
				}
				else
				{
					// TODO : 문제 있는 상황
					break;
				}

			}

			cout << "Data Recv Len = " << recvLen << endl;
		}

		::closesocket(session.socket);
		::WSACloseEvent(wsaEvent);
	}

	// 원속 종료
	::WSACleanup();
}
*/
// Overlapped Model (event base)
/*
int main()
{
	WSAData wsaData;
	if (::WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
		return 0;

	SOCKET listenSocket = ::socket(AF_INET, SOCK_STREAM, 0);
	if (listenSocket == INVALID_SOCKET)
		return 0;

	u_long on = 1;
	if (::ioctlsocket(listenSocket, FIONBIO, &on) == INVALID_SOCKET)
		return 0;

	SOCKADDR_IN serverAddr;
	::memset(&serverAddr, 0, sizeof(serverAddr));
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_addr.s_addr = ::htonl(INADDR_ANY);
	serverAddr.sin_port = ::htons(7777);

	if (::bind(listenSocket, (SOCKADDR*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR)
		return 0;

	if (::listen(listenSocket, SOMAXCONN) == SOCKET_ERROR)
		return 0;

	cout << "Accept" << endl;

	// Overlapped IO (비동기 + 논블로킹)
	// - Overlapped 함수를 건다 (WSARecv, WSASend)
	// - Overlapped 함수가 성공했는지 확인 후
	// -> 성공했으면 결과 얻어서 처리
	// -> 실패했으면 사유를 확인(에러인지 미완료 상태인지)

	//char sendBuffer[100];
	//WSABUF wsaBuf[2];
	//wsaBuf[0].buf = sendBuffer;
	//wsaBuf[0].len = 100;

	//char sendBuffer2[100];
	//wsaBuf[1].buf = sendBuffer2;
	//wsaBuf[1].len = 100;

	// 1) 비동기 입출력 소켓
	// 2) WSABUF 구조체 배열의 시작 주소 + 개수
	// 3) 보내고/받은 데바이트 수
	// 4) 상세 옵션인데 0
	// 5) WSAOVERLAPPED 구조체 주소값, 
	// 구조체의 handle에 설정 된 이벤트로 인해 운영체제가 signal 상태로 바꾸면, 그것을 탐지 완료를 판별
	// 6) 입출력이 완료되면 OS가 호출할 콜백 함수
	// WSASend
	// WSARecv
	
	// Overlapped 모델 (이벤트 기반)
	// - 비동기 입출력 지원하는 소켓 생성 + 통지 받기 위한 이벤트 객체 생성
	// - 비동기 입출력 함수 호출 (앞에서 만든 이벤트 객체를 같이 넘겨줌)
	// - 비동기 작업이 바로 완료되지 않으면, WSA_IO_PENDING 오류 코드
	// 운영체제는 이벤트 객체를 signaled 상태로 만들어서 완료 상태 알려줌
	// - WSAWaitForMultipleEvents 함수 호출해서 이벤트 객체의 signal 판별
	// - WSAGetOverlappedResult 호출해서 비동기 입출력 결과 확인 및 데이터 처리

	// 1) 비동기 소켓
	// 2) 넘겨준 overlapped 구조체
	// 3) 전송된 바이트 수
	// 4) 비동기 입출력 작업이 끝날 때까지 대기할지? : 이미 완료된 상태에서 호출하므로 false
	// 5) 비동기 입출력 작업 관련 부가 정보. 거의 사용 안함
	// WSAGetOverlappedResult

	while (true)
	{
		SOCKADDR_IN clientAddr;
		int32 addrLen = sizeof(clientAddr);

		SOCKET clientSocket;
		while (true)
		{
			// listenSocke이 논블록이므로 clientSocket도 논블록
			clientSocket = ::accept(listenSocket, (SOCKADDR*)&clientAddr, &addrLen);
			if (clientSocket != INVALID_SOCKET)
				break;

			if (::WSAGetLastError() == WSAEWOULDBLOCK)
				continue;

			//문제 있는 상황
			return 0;
		}

		Session session = Session{ clientSocket };
		WSAEVENT wsaEvent = ::WSACreateEvent();
		session.overlapped.hEvent = wsaEvent;

		cout << "Client Connected !" << endl;

		while (true)
		{
			WSABUF wsaBuf;
			wsaBuf.buf = session.recvBuffer;
			wsaBuf.len = BUFSIZE;

			DWORD recvLen = 0;
			DWORD flags = 0;
			//recv를 비동기 방식으로 호출
			if (::WSARecv(clientSocket, &wsaBuf, 1, &recvLen, &flags, &session.overlapped, nullptr) == SOCKET_ERROR)
			{
				if (::WSAGetLastError() == WSA_IO_PENDING)
				{
					// Pending 그리고 대기
					::WSAWaitForMultipleEvents(1, &wsaEvent, TRUE, WSA_INFINITE, FALSE);
					// 결과를 가져 옴 
					::WSAGetOverlappedResult(session.socket, &session.overlapped, &recvLen, FALSE, &flags);
				}
				else
				{
					// TODO : 문제 있는 상황
					break;
				}
					
			}

			cout << "Data Recv Len = " << recvLen << endl;
		}

		::closesocket(session.socket);
		::WSACloseEvent(wsaEvent);
	}

	// 원속 종료
	::WSACleanup();
}
*/
// WSAEventSelect Model
/*
int main()
{
	WSAData wsaData;
	if (::WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
		return 0;

	SOCKET listenSocket = ::socket(AF_INET, SOCK_STREAM, 0);
	if (listenSocket == INVALID_SOCKET)
		return 0;

	u_long on = 1;
	if (::ioctlsocket(listenSocket, FIONBIO, &on) == INVALID_SOCKET)
		return 0;

	SOCKADDR_IN serverAddr;
	::memset(&serverAddr, 0, sizeof(serverAddr));
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_addr.s_addr = ::htonl(INADDR_ANY);
	serverAddr.sin_port = ::htons(7777);

	if (::bind(listenSocket, (SOCKADDR*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR)
		return 0;

	if (::listen(listenSocket, SOMAXCONN) == SOCKET_ERROR)
		return 0;

	cout << "Accept" << endl;

	// WSAEventSelect = (WSAEventSelect 함수가 핵심이 되는, window 함수)
	// 소켓과 관련된 네트워크 이벤트를 [이벤트 객체]를 통해 감지
	// select는 동기 방식인데 비해, WSAEventSelect는 이벤트 객체를 통한 비동기 방식

	// 이벤트 객체 관련 함수들
	// 생성 : WSACreateEvent ( 수동 리셋 Manual-Reset + Non-Signaled 상태 시작)
	// 삭제 : WSACloseEvent
	// 신호 상태 감지 : WSAWaitForMultipleEvents
	// 구체적인 네트워크 이벤트 알아내기 : SWAEnumNetWorkEvents
	
	// 소켓 <-> 이벤트 객체 연동
	// soket을 event를 통해 어떤 networkEvents 상태를 감지하고 싶은지 등록
	// WSAEventSelect(socket, event, networkEvents); 
	// 관심 있는 네트워크 이벤트(networkEvents)
	// FD_ACCEPT : 접속한 클라가 있음 accept
	// FD_READ : 데이터 수신 가능 recv, recvfrom
	// FD_WRITE : 데이터 송신 가능 send, sendto,
	// FD_CLOSE : 상대가 접속 종료
	// FD_CONNECT : 통신을 위한 연결 절차 완료
	// FD_OOB :

	// 주의 사항
	// WSAEventSelect 함수를 호출하면, 해당 소켓은 자동으로 넌블로킹 모드 전환
	// accept() 함수가 리턴하는 소켓은 listenSocket과 동일한 속성을 갖는다.
	// - 따라서 clientSocket 은 FD_READ, FD_WRITE 등을 다시 등록 필요
	// - 드물게 WSAEWOULDBLOCK 오류가 뜰 수 있으니 예외 처리 필요
	// 중요)
	// - 이벤트 발생 시, 적절한 소켓 함수 호출해야 함
	// - 아니면 다음 번에는 동일 네트워크 이벤트 발생 X
	// ex) FD_READ 이벤트 떳으면 recv() 호출해야 하고, 안하면 FD_READ 두 번 다시 X

	// 1) count, event
	// 2) waitAll : 모두 기다리기, 하나만 완료 기다리기
	// 3) timeout : 타임아웃
	// 4) 지금은 false
	// return : 완료된 첫번째 인덱스
	// WSAWaitForMultipleEvents

	// 1) socket
	// 2) eventObject : socket 과 연동된 이벤트 객체 핸들을 넘겨주면, 이벤트 객체를 non-signaled
	// 3) networkEvent : 네트워크 이벤트 / 오류 정보가 저장
	// WSANumNetworkEvents
	
	// WSAEVENT와 Session
	vector<WSAEVENT> wsaEvents;
	vector<Session> sessions; // Session 관리하는 벡터
	sessions.reserve(100);

	WSAEVENT listenEvent = ::WSACreateEvent();
	wsaEvents.push_back(listenEvent);
	// client와 연결되는 소켓은 아니지만, 섹션 벡터와 이벤트 벡터의 인덱스를 같게 하기 위해
	sessions.push_back(Session{ listenSocket });

	if (::WSAEventSelect(listenSocket, listenEvent, FD_ACCEPT | FD_CLOSE) == SOCKET_ERROR)
		return 0;

	while (true)
	{
		// wsaEvents 중 완료된 인덱스의 이벤트 먼저 처리하면서 루프
		int32 index = ::WSAWaitForMultipleEvents(wsaEvents.size(), &wsaEvents[0], FALSE, WSA_INFINITE, FALSE);
		if (index == WSA_WAIT_FAILED)
			continue;

		// wsaEvents 중 완료된 이벤트의 인덱스 번호를 연산
		index -= WSA_WAIT_EVENT_0;

		//::WSAResetEvent(wsaEvents[index]);
		// 이벤트를 리셋 시켜야 하나, WSAEnumNetworkEvents에서 초기화 시켜 줌

		WSANETWORKEVENTS networkEvents;
		// 관찰하려는 소켓의 이벤트 내용이 networkEvents 구조체에 저장
		if (::WSAEnumNetworkEvents(sessions[index].socket, wsaEvents[index], &networkEvents) == SOCKET_ERROR)
			continue;

		// Listener 소켓 체크
		if (networkEvents.lNetworkEvents & FD_ACCEPT)
		{
			// Error-Check
			if (networkEvents.iErrorCode[FD_ACCEPT_BIT] != 0)
				continue;

			SOCKADDR_IN clientAddr;
			int32 addrLen = sizeof(clientAddr);

			SOCKET clientSocket = ::accept(listenSocket, (SOCKADDR*)&clientAddr, &addrLen);
			if (clientSocket != INVALID_SOCKET)
			{
				cout << "Client Connected" << endl;

				WSAEVENT clientEvent = ::WSACreateEvent();
				wsaEvents.push_back(clientEvent);
				sessions.push_back(Session{ clientSocket });
				if (::WSAEventSelect(clientSocket, clientEvent, FD_READ | FD_WRITE | FD_CLOSE) == SOCKET_ERROR)
					return 0;
			}
		}

		// Client Session 소켓 체크
		if (networkEvents.lNetworkEvents & FD_READ || networkEvents.lNetworkEvents & FD_WRITE)
		{
			// Error-Check
			if ((networkEvents.lNetworkEvents & FD_READ) && networkEvents.iErrorCode[FD_READ_BIT] != 0)
				continue;
			// Error0Check
			if ((networkEvents.lNetworkEvents & FD_WRITE) && networkEvents.iErrorCode[FD_WRITE_BIT] != 0)
				continue;

			Session& s = sessions[index];

			// Read
			if (s.recvBytes == 0)
			{
				int32 recvLen = ::recv(s.socket, s.recvBuffer, BUFSIZE, 0);
				if (recvLen == SOCKET_ERROR && ::WSAGetLastError() != WSAEWOULDBLOCK)
				{
					// 문제가 있는 상황
					// TODO : Remove Session
					continue;
				}

				// 실행 가능
				s.recvBytes = recvLen;
				cout << "Recv Data = " << recvLen << endl;
			}

			// Write
			// recvBytes가 sendBytes 보다 커서 보낼 게 남았을 때
			if (s.recvBytes > s.sendBytes)
			{
				int32 sendLen = ::send(s.socket, &s.recvBuffer[s.sendBytes], s.recvBytes - s.sendBytes, 0);
				if (sendLen == SOCKET_ERROR && ::WSAGetLastError() != WSAEWOULDBLOCK)
				{
					// 문제가 있는 상황
					// TODO : Remove Session
					continue;
				}

				// 실행 가능
				s.sendBytes += sendLen;
				if (s.recvBytes == 0)
				{
					s.recvBytes = 0;
					s.sendBytes = 0;
				}

				cout << "Send Data = " << sendLen << endl;
			}
		}

		// FD_CLOSE 처리
		// 연결이 끊겼을 때도 대비
		if (networkEvents.lNetworkEvents & FD_CLOSE)
		{
			// TODO : Remove Socket
		}

	}

	// 원속 종료
	::WSACleanup();
}
*/
// Select 모델
/*
int main()
{
	WSAData wsaData;
	if (::WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
		return 0;

	// 블로킹(Blocking) 소켓
	// accept -> 접속한 클라가 있을 때
	// connect -> 서버 접속 성공했을 때
	// send, sendto -> 요청한 데이터를 송신 버퍼에 복사했을 때
	// recv, recvfrom -> 수신 버퍼에 도착한 데이터가 있고, 이를 유저레벨 버퍼에 복사했을 떄 성공

	// 논블로킹(Non-Blocking)

	SOCKET listenSocket = ::socket(AF_INET, SOCK_STREAM, 0);
	if (listenSocket == INVALID_SOCKET)
	{
		HandleError("Socket");
		return 0;
	}

	u_long on = 1;
	// 논블로킹 소켓으로 변환
	if (::ioctlsocket(listenSocket, FIONBIO, &on) == INVALID_SOCKET)
		return 0;

	SOCKADDR_IN serverAddr;
	::memset(&serverAddr, 0, sizeof(serverAddr));
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_addr.s_addr = ::htonl(INADDR_ANY);
	serverAddr.sin_port = ::htons(7777);

	if (::bind(listenSocket, (SOCKADDR*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR)
		return 0;

	if (::listen(listenSocket, SOMAXCONN) == SOCKET_ERROR)
		return 0;

	cout << "Accept" << endl;

	// Select 모델 = (select 함수가 핵심이 되는)
	// 소켓 함수 호출이 성공할 시점을 미리 알 수 있다!
	// 클라이언트같은 적은 버퍼 환경에서 유용
	// 문제 상황)
	// 수신버퍼에 데이터가 없는데, read 한다거나
	// 송신버퍼가 꽉 찼는데, write 한다거나
	// - 블로킹 소켓 : 조건이 만족되지 않아서 블로킹되는 상황 예방
	// - 논블로킹 소켓 : 조건이 만족되지 않아서 불필요하게 반복 쳌하는 상황을 예방

	// socket set // 읽기 또는 쓰기 소켓에 대한 원하는 시점을 관찰
	// 1) 읽기[ ] 쓰기[ ] 예외(00B)[ ] 관찰 대상 등록
	// OutOfBand는 send() 마지막 인자 MSG_OOB로 보내는 특별한 데이터
	// 받는 쪽에서도 recv 00B 세팅을 해야 읽을 수 있음 <-- 사실상 예외 소켓 셋은 잘 사용하지 않음
	// 2) select(readSet, writeSet, exceptSet) -> 세 socket set 인자를 받고 관찰 시작
	// 3) 적어도 하나의 소켓이 준비되면 리턴 -> 낙오자는 알아서 제거됨
	// 4) 남은 소켓 체크해서 진행 -> 낙오된 소켓은 소켓 셋에 계속 채워줘서 select

	// fd_set set;
	// FD_ZERO(&set) : 소켓 셋 비우기
	// FD_SET(s, &set) : 소켓 셋에 소켓 s 넣기
	// FD_CLR(s, &set) : 소켓 셋에 소켓 s 제거
	// FD_ISSET(s, &set) : 소켓 s가 set에 들어 있으면 0이 아닌 값 리턴

	vector<Session> sessions; // Session 관리하는 벡터
	sessions.reserve(100);

	fd_set reads;
	fd_set writes;

	while (true)
	{
		// 소켓 셋 초기화
		FD_ZERO(&reads);
		FD_ZERO(&writes);

		// ListenSocket 등록
		FD_SET(listenSocket, &reads); // 처음 accept 하기 위해

		// 알맞게 소켓 등록
		for (Session& s : sessions)
		{
			if (s.recvBytes <= s.sendBytes)
				FD_SET(s.socket, &reads); // 읽고 보내야 하므로
			else
				FD_SET(s.socket, &writes);
		}

		// [옵션] 마지막 timeout 인자 설정 가능
		// 마지막 인자인 timeval을 넣지 않으면 하나의 set 이라도 준비될 때까지 대기
		timeval timeout;
		//timeout.tv_sec;
		//timeout.tv_usec;
		int32 retVal = ::select(0, &reads, &writes, nullptr, nullptr);
		if (retVal == SOCKET_ERROR)
			break;
		// select 함수에서 하나라도 준비되면 리턴하며 reads와 writes에서 낙오자는 알아서 제거


		// Listener 소켓 체크
		if (FD_ISSET(listenSocket, &reads))
		{
			SOCKADDR_IN clientAddr;
			int32 addrLen = sizeof(clientAddr);
			SOCKET clientSocket = ::accept(listenSocket, (SOCKADDR*)&clientAddr, &addrLen);
			if (clientSocket != INVALID_SOCKET) //select함수로 실행 가능한 소켓이 있으므로
			{
				cout << "Client Connected" << endl;
				sessions.push_back(Session{ clientSocket });
			}
		}

		// listenSocket 이외에 나머지 소켓 체크
		for (Session& s : sessions)
		{
			// Read 체크
			if (FD_ISSET(s.socket, &reads))
			{
				int32 recvLen = ::recv(s.socket, s.recvBuffer, BUFSIZE, 0);
				if (recvLen <= 0)
				{
					// TODO : sessions 제거
					continue;
				}

				s.recvBytes = recvLen;
			}

			// Write 체크
			if(FD_ISSET(s.socket, &writes))
			{
				// 블로킹 모드였을 경우 -> 모든 데이터 다 보냄
				// 논블로킹 모드 -> 일부만 보낼 수 있음 (상대방 수신 버퍼 상황에 따라) 
				// 
				::send(s.socket, &s.recvBuffer[s.sendBytes], s.recvBytes - s.sendBytes, 0);
			}
		}
	}

	// 원속 종료
	::WSACleanup();
}
*/
// non blocking socket
/*
int main()
{
	WSAData wsaData;
	if (::WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
		return 0;

	// 블로킹(Blocking) 소켓
	// accept -> 접속한 클라가 있을 때
	// connect -> 서버 접속 성공했을 때
	// send, sendto -> 요청한 데이터를 송신 버퍼에 복사했을 때
	// recv, recvfrom -> 수신 버퍼에 도착한 데이터가 있고, 이를 유저레벨 버퍼에 복사했을 떄 성공

	// 논블로킹(Non-Blocking)

	SOCKET listenSocket = ::socket(AF_INET, SOCK_STREAM, 0);
	if (listenSocket == INVALID_SOCKET)
	{
		HandleError("Socket");
		return 0;
	}

	u_long on = 1;
	// 논블로킹 소켓으로 변환
	if (::ioctlsocket(listenSocket, FIONBIO, &on) == INVALID_SOCKET)
		return 0;

	SOCKADDR_IN serverAddr;
	::memset(&serverAddr, 0, sizeof(serverAddr));
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_addr.s_addr = ::htonl(INADDR_ANY);
	serverAddr.sin_port = ::htons(7777);

	if (::bind(listenSocket, (SOCKADDR*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR)
		return 0;

	if (::listen(listenSocket, SOMAXCONN) == SOCKET_ERROR)
		return 0;

	cout << "Accept" << endl;

	SOCKADDR_IN clientAddr;
	int32 addrLen = sizeof(clientAddr);

	// accept 를 위한 반복
	while (true)
	{
		SOCKET clientSocket = ::accept(listenSocket, (SOCKADDR*)&clientAddr, &addrLen);
		if (clientSocket == INVALID_SOCKET)
		{
			//accept 이나 recv로 인한 블록이 일어날만한 상황
			if (::WSAGetLastError() == WSAEWOULDBLOCK)
				continue;

			//Error
			break;
		}

		cout << "Client Connected!" << endl;

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

			// Send
			while (true)
			{
				if (::send(clientSocket, recvBuffer, recvLen, 0) == SOCKET_ERROR)
				{
					// 논블록킹이 일어날만한 상황
					if (::WSAGetLastError() == WSAEWOULDBLOCK)
						continue;

					//Error
					break;
				}

				cout << "Send Data ! Len = " << recvLen << endl;
				break;
			}
		}
	}

	// 원속 종료
	::WSACleanup();
}
*/
// socket option
/*
int main()
{
	WSAData wsaData;
	if (::WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
		return 0;

	SOCKET serverSocket = ::socket(AF_INET, SOCK_DGRAM, 0);
	if (serverSocket == INVALID_SOCKET)
	{
		HandleError("Socket");
		return 0;
	}

	// 레벨: 옵션을 해석하고 처리할 주체
	//-----------레벨설정
	// 소켓 코드 -> SOL_SOCKET
	// IPv4 -> IPPROTO_OP
	// TCP 프로토콜 -> IPPROTO_TCP
	//-------------------
	// 해당 레벨의 옵션도 설정

	//----------레벨옵션
	// SO_KEEPALIVE: 주기적으로 연결 상태 확인 여부 (TCP only)
	// 상대방이 임의로 연결을 끊을 수도 있으므로
	// 주기적으로 TCP 프로토콜 연결 상태 확인 -> 끊어진 연결 감지
	//------------------
	bool enable = true;
	::setsockopt(serverSocket, SOL_SOCKET, SO_KEEPALIVE, (char*)&enable, sizeof(enable));

	//----------레벨옵션
	// SO_LINGER : 지연
	// closesocket 할 시, 송신 버퍼에 있는 데이터를 보낼 것인가? 날릴 것인가?
	// if onoff = 0, closesoket() 바로 리턴, else linger초만큼 대기(default 0)
	// linger : 대기 시간
	//------------------
	LINGER linger;
	linger.l_onoff = 1;
	linger.l_linger = 5;
	::setsockopt(serverSocket, SOL_SOCKET, SO_LINGER, (char*)&linger, sizeof(linger));

	// Half-Close
	// SD_SEN :send 막음
	// SD_RECEIVE : recv 막음
	// SD_BOTH : 둘다 반환
	// closesocket 대신, 적용
	//::shutdown(serverSocket, SD_SEND);

	// 소켓 리소스 반환
	// sned -> closesocket 할 시, linger 옵션에 따라 다름
	//::closesocket(serverSocket);

	// SO_SNDBUF : 송신 버퍼 크기
	// SO_RCVBUF : 수신 버퍼 크기

	int32 sendBufferSize;
	int32 optionLen = sizeof(sendBufferSize);
	::getsockopt(serverSocket, SOL_SOCKET, SO_SNDBUF, (char*)&sendBufferSize, &optionLen);
	cout << "송신 버퍼 크기 : " << sendBufferSize << endl;

	int32 recvBufferSize;
	optionLen = sizeof(recvBufferSize);
	::getsockopt(serverSocket, SOL_SOCKET, SO_RCVBUF, (char*)&recvBufferSize, &optionLen);
	cout << "수신 버퍼 크기 : " << recvBufferSize << endl;

	//----------레벨옵션
	// SO_REUSEADDR : IP 주소 및 port 재사용
	// 강제로 소켓이 종료될 시, 다음 실행에 시간 격차를 두어야 무사히 소켓 사용가능
	// --> 강제로 재사용할 수 있음
	//------------------
	{
		bool enable = true;
		::setsockopt(serverSocket, SOL_SOCKET, SO_REUSEADDR, (char*)&enable, sizeof(enable));
	}

	// IPPROTO_TCP
	// TCP_NODELAY = Nagle 네이글 알고리즘 작동 여부
	// 데이터가 충분히 크면 보내고, 그렇지 않으면 데이터가 충분히 쌓일 때까지 대기
	// 장점: 작은 패킷이 불필요하게 많이 생성되는 일을 방지
	// 단점: 반응 시간 손해
	{
		// true가 네이글 알고리즘 off
		bool enable = true;
		::setsockopt(serverSocket, IPPROTO_TCP, TCP_NODELAY, (char*)&enable, sizeof(enable));
	}

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

	SOCKET serverSocket = ::socket(AF_INET, SOCK_DGRAM, 0);
	if (serverSocket == INVALID_SOCKET)
	{
		HandleError("Socket");
		return 0;
	}

	SOCKADDR_IN serverAddr; // IPv4 구조체
	::memset(&serverAddr, 0, sizeof(serverAddr)); // 구조체 초기화
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_addr.s_addr = ::htonl(INADDR_ANY); // 자신의 아이피로 알아서 지정
	serverAddr.sin_port = ::htons(7777); //포트의 임의 번호 지정

	if (::bind(serverSocket, (SOCKADDR*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR)
	{
		HandleError("Bind");
		return 0;
	}

	while (true)
	{
		SOCKADDR_IN clientAddr;
		::memset(&clientAddr, 0, sizeof(clientAddr));
		int32 addrLen = sizeof(clientAddr);

		char recvBuffer[1000];

		//패킷을 보낸 client의 주소와 버퍼를 받아옴
		int32 recvLen = ::recvfrom(serverSocket, recvBuffer, sizeof(recvBuffer), 0,
			(SOCKADDR*)&clientAddr, &addrLen);

		if (recvLen <= 0)
		{
			HandleError("RecvFrom");
			return 0;
		}

		cout << "Recv Data! Data = " << recvBuffer << endl;
		cout << "Recv Data! Len = " << recvLen << endl;

		int32 errorCode = ::sendto(serverSocket, recvBuffer, recvLen, 0, 
			(SOCKADDR*)&clientAddr, sizeof(clientAddr));

		if (errorCode == SOCKET_ERROR)
		{
			HandleError("SendTo");
			return 0;
		}

		cout << "Send Data! Len = " << recvLen << endl;
	}

	// 원속 종료
	::WSACleanup();
}
*/
// TCP
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
	SOCKET listenSocket = ::socket(AF_INET, SOCK_STREAM, 0);
	if (listenSocket == INVALID_SOCKET)
	{
		int32 errCode = ::WSAGetLastError(); // 어떤 이유로 안되는지 에러 코드를 받아옴
		cout << "Soket ErrorCode : " << errCode << endl;
		return 0;
	}

	// 나의 주소는?: IP주소 + Port
	SOCKADDR_IN serverAddr; // IPv4 구조체
	::memset(&serverAddr, 0, sizeof(serverAddr)); // 구조체 초기화
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_addr.s_addr = ::htonl(INADDR_ANY); // 자신의 아이피로 알아서 지정
	serverAddr.sin_port = ::htons(7777); //포트의 임의 번호 지정/ 일부포트는 예약되어 있음

	// 안내원 연결
	if (::bind(listenSocket, (SOCKADDR*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) //서버와 소켓을 연동
	{
		int32 errCode = ::WSAGetLastError(); // 어떤 이유로 안되는지 에러 코드를 받아옴
		cout << "Bind ErrorCode : " << errCode << endl;
		return 0;
	}

	// 영업 시작
	if (::listen(listenSocket, 10) == SOCKET_ERROR) //10: 백로그, 대기열 한도, 대기열을 넘을 시 접근 거부
	{
		int32 errCode = ::WSAGetLastError(); // 어떤 이유로 안되는지 에러 코드를 받아옴
		cout << "Listen ErrorCode : " << errCode << endl;
		return 0;
	}

	// --------------------
	// 통과

	while (true)
	{
		// 자신에게 연결하는 클라이언트 주소
		SOCKADDR_IN clientAddr; // IPv4
		::memset(&clientAddr, 0, sizeof(clientAddr)); // 구조체 초기화
		int32 addrLen = sizeof(clientAddr);
		// 자신에게 연결한 클라이언트의 주소를 넘김
		// listenSocket 은 처음 클라이언트가 연결할 떄만 사용되고, 나머지 통신은 clientSocket에서 처리함
		SOCKET clientSocket = ::accept(listenSocket, (SOCKADDR*)&clientAddr,&addrLen);
		if (clientSocket == INVALID_SOCKET)
		{
			int32 errCode = ::WSAGetLastError(); // 어떤 이유로 안되는지 에러 코드를 받아옴
			cout << "Accept ErrorCode : " << errCode << endl;
			return 0;
		}

		// 손님 임장!
		char ipAddress[16];
		::inet_ntop(AF_INET, &clientAddr.sin_addr, ipAddress, sizeof(ipAddress));
		cout << "Client Connected! IP = " << ipAddress << endl;

		// TODO
		while (true)
		{
			// 클라이언트에서 보내는 버퍼의 크기를 알기 어려움으로 넉넉하게 공간을 잡음
			char recvBuffer[1000];

			this_thread::sleep_for(1s);

			int32 recvLen = ::recv(clientSocket, recvBuffer, sizeof(recvBuffer), 0);
			if (recvLen <= 0)
			{
				int32 errCode = ::WSAGetLastError();
				cout << "Recv ErrorCode : " << errCode << endl;
				return 0;
			}

			cout << "Recv Data! Data = " << recvBuffer << endl;
			cout << "Recv Data! Len = " << recvLen << endl;

			// 소켓에 전송할 버퍼와 크기를 송신
			int32 resultCode = ::send(clientSocket, recvBuffer, recvLen, 0);
			if (resultCode == SOCKET_ERROR)
			{
				int32 errCode = ::WSAGetLastError();
				cout << "Send ErrorCode : " << errCode << endl;
				return 0;
			}
		}
	}

	// --------------------

	// 원속 종료
	::WSACleanup();
}
*/
//TypeCast
/*
#include "RefCounting.h"
#include "Memory.h"

using TL = TypeList<class Player, class Mage, class Knight, class Archer>;

class Player
{
public:
	Player()
	{
		INIT_TL(Player)
	}
	virtual void MyType()
	{
		cout << "Player" << endl;
	}
	DECLARE_TL;
};

class Knight : public Player
{
public:
	Knight() {INIT_TL(Knight) }
	void MyType()
	{
		cout << "Knight" << endl;
	}
	int32 stack[10];
};

class Mage : public Player
{
public:
	Mage() {INIT_TL(Mage) }
	void MyType()
	{
		cout << "Mage" << endl;
	}
};

class Archer : public Player
{
public:
	Archer() {INIT_TL(Archer) }
	void MyType()
	{
		cout << "Archer" << endl;
	}
};

class Dog
{
public:
	//Dog() {INIT_TL(Dog) }
};

int main()
{
	TypeList<Mage, TypeList<Knight, Archer>>::Tail::Tail player;

	int32 len1 = Length<TypeList<Mage, Knight, Archer>>::value;

	TypeAt<TL, 2>::Result type2;
	TypeAt<TL, 0>::Result type0;

	int32 indexMage = IndexOf<TL, Mage>::value;
	int32 indexKnight = IndexOf<TL, Knight>::value;
	int32 indexDog = IndexOf<TL, Dog>::value;

	bool exist1 = Conversion<Knight, Player>::exists;
	bool exist2 = Conversion<Player, Knight>::exists;
	bool exist3 = Conversion<Knight, Dog>::exists;

	TypeConversion<TL> test;

	test.s_convert;

	{
		Player* player = new Knight();

		bool canCast = CanCast<Knight*>(player);
		Knight* knight = TypeCast<Knight*>(player);

		cout << (canCast ? "true" : "false") << sizeof(*knight) << endl;
		knight->MyType();
	}
	{
		Knight* player = new Knight();

		bool canCast = CanCast<Player*>(player);
		Player* knight = TypeCast<Player*>(player);

		cout << (canCast ? "true" : "false") << endl;
		knight->MyType();
	}
	{
		shared_ptr<Knight> knight = MakeShared<Knight>();
		shared_ptr<Player> player = TypeCast<Player>(knight);
	}
}
*/

//Memory Pool #3
/*
class Knight
{
public:
	int32 _hp = rand() % 1000;
};

class Monster
{
public:
	int32 _id = 0;
};

int main()
{
	Knight* knights[100];

	for (int32 i = 0; i < 100; i++)
	{
		knights[i] = ObjectPool<Knight>::Pop();
	}

	for (int32 i = 0; i < 100; i++)
	{
		ObjectPool<Knight>::Push(knights[i]);
		knights[i] = nullptr;
	}

	//Knight* k = ObjectPool<Knight>::Pop();
	//ObjectPool<Knight>::Push(k);
	// ## delete k 등으로 데이터가 직접 지워질 수 있음
	// ## 그러므로 따로 관리함

	// shared_ptr<Knight> sptr = { ObjectPool<Knight>::Pop() , ObjectPool<Knight>::Push };
	shared_ptr<Knight> sptr = ObjectPool<Knight>::MakeShared();
	shared_ptr<Knight> sptr2 = MakeShared<Knight>();

	for (int32 i = 0; i < 5; i++)
	{
		GThreadManager->Launch([]()
			{
				while (true)
				{
					Knight* knight = xnew<Knight>();
					cout << knight->_hp << endl;
					this_thread::sleep_for(10ms);
					xdelete(knight);
				}
			});
	}
}
*/
//class Data
//{
//public:
//	SLIST_ENTRY entry;
//	int64 _rand = rand() % 1000;
//};
//
//SLIST_HEADER* GHeader;
//
//int main()
//{
//	GHeader = new SLIST_HEADER();
//	ASSERT_CRASH(((uint64)GHeader % 16) == 0);
//	::InitializeSListHead(GHeader);
//
//	for (int32 i = 0; i < 3; i++)
//	{
//		GThreadManager->Launch([]()
//			{
//				while (true)
//				{
//					Data* data = new Data();
//					ASSERT_CRASH(((uint64)data % 16) == 0);
//
//					::InterlockedPushEntrySList(GHeader, (SLIST_ENTRY*)data);
//					this_thread::sleep_for(10ms);
//				}
//			});
//	}
//
//	for (int32 i = 0; i < 2; i++)
//	{
//		GThreadManager->Launch([]()
//			{
//				while (true)
//				{
//					Data* pop = new Data();
//					pop = (Data*)::InterlockedPopEntrySList(GHeader);
//
//					if (pop)
//					{
//						cout << pop->_rand << endl;
//						delete pop;
//					}
//					else
//					{
//						cout << "NONE" << endl;
//					}
//				}
//			});
//	}
//
//	GThreadManager->Join();
//}

// Memory Pool #2
/*
#include "ConcurrentStack.h"

//class Data // : public SListEntry
//{
//public:
//	SListEntry _entry;
//
//	int32 _hp;
//	int32 _mp;
//};

class Data
{
	
public:
	SListEntry _n;
	int64 _rand = rand() % 1000;
};

SListHeader* GHeader;

int main()
{
	//SListHeader header;
	//InitializeHead(&header);

	//Data* data = new Data();
	//data->_hp = 10;
	//data->_mp = 20;

	//PushEntrySList(&header, (SListEntry*)data); //data가 SListEntry를 포함하므로 포인터 치환이 가능하다

	//Data* popData = (Data*)PopEntrySList(&header);

	GHeader = new SListHeader();
	ASSERT_CRASH(((uint64)GHeader % 16) == 0);
	InitializeHead(GHeader);

	for (int32 i = 0; i < 3; i++)
	{
		GThreadManager->Launch([]()
			{
				while (true)
				{
					Data* data = new Data();
					ASSERT_CRASH(((uint64)data % 16) == 0);

					PushEntrySList(GHeader, (SListEntry*)data);
					this_thread::sleep_for(10ms);
				}
			});
	}

	for (int32 i = 0; i < 2; i++)
	{
		GThreadManager->Launch([]()
			{
				while (true)
				{
					Data* pop = new Data();
					pop = (Data*)PopEntrySList(GHeader);

					if (pop)
					{
						cout << pop->_rand << endl;
						delete pop;
					}
					else
					{
						cout << "NONE" << endl;
					}
				}
			});
	}

	GThreadManager->Join();
}
*/

// Memory Pool #1
/*
class Player
{
public:
	Player() {}
	virtual ~Player() {}
};

class Knight : public Player
{
public:
	Knight()
	{
		cout << "Knight()" << endl;
	}

	Knight(int32 hp) : _hp(hp)
	{
		cout << "Knight(hp)" << endl;
	}

	~Knight()
	{
		cout << "~Knight()" << endl;
	}

	int32 _hp = 100;
	int32 _mp = 10;
};

int main()
{
	for (int32 i = 0; i < 5; i++) {
		GThreadManager->Launch([]()
			{
				while (true)
				{
					xVector<Knight> v(100);
					
					xMap<int32, Knight> m;
					m[100] = Knight();

					this_thread::sleep_for(10ms);
				}
			});
	}

	GThreadManager->Join();
}
*/
// STLAllocator
/*
int main()
{
	xVector<Knight> v(100);
	
	xMap<int32, Knight> m;
	m[100] = Knight();

}
*/
// Allocator & StompAllocator
/*
class Player
{
public:
	Player() {}
	virtual ~Player() {}
};

class Knight : public Player
{
public:
	Knight()
	{
		cout << "Knight()" << endl;
	}

	Knight(int32 hp) : _hp(hp)
	{
		cout << "Knight()" << endl;
	}

	~Knight() 
	{
		cout << "~Knight()" << endl;
	}

	//static void* operator new(size_t size)
	//{
	//	cout << "new! " << size << endl;
	//	void* ptr = ::malloc(size);
	//	return ptr;
	//}

	//static void operator delete(void* ptr)
	//{
	//	cout << "delete! " << endl;
	//	::free(ptr);
	//}

	//static void* operator new[](size_t size)
	//{
	//	cout << "new[]! " << size << endl;
	//	void* ptr = ::malloc(size);
	//	return ptr;
	//}

	//static void operator delete[](void* ptr)
	//{
	//	cout << "delete[]! " << endl;
	//	::free(ptr);
	//}

	int32 _hp = 100;
	int32 _mp = 10;
};

// new operator overloading (Global)
//void* operator new(size_t size)
//{
//	cout << "new! " << size << endl;
//	void* ptr = ::malloc(size);
//	return ptr;
//}
//
//void operator delete(void* ptr)
//{
//	cout << "delete! " << endl;
//	::free(ptr);
//}
//
//void* operator new[](size_t size)
//{
//	cout << "new[]! " << size << endl;
//	void* ptr = ::malloc(size);
//	return ptr;
//}
//
//void operator delete[](void* ptr)
//{
//	cout << "delete[]! " << endl;
//	::free(ptr);
//}

int main()
{
	//Knight* k1 = new Knight();
	//k1->_hp = 200;
	//k1->_mp = 50;
	//delete k1;
	//k1->_hp = 100;	//잘못된 메모리 접근

	//Player* p = new Player();
	//Knight* k = static_cast<Knight*>(p);
	//k->_hp = 200;		//잘못된 멤버변수에 접근

	//// 가상 메모리 기본
	//int* num = new int;
	//*num = 100;
	//int64 address = reinterpret_cast<int64>(&num);
	//cout << address << endl;

	//int* num2 = reinterpret_cast<int*>(525471840008); //이전 address의 값
	//*num2 = 200; //접근 거부
	//delete num;

	//SYSTEM_INFO info;
	//::GetSystemInfo(&info);

	//info.dwPageSize; // 4KB (0x1000)
	//info.dwAllocationGranularity; // 64KB (0x1000)

	//Knight* k1 = new Knight();
	//int* test = (int*)::VirtualAlloc(NULL, 4, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
	//*test = 100;
	//::VirtualFree(test, 0, MEM_RELEASE);
	//*test = 200;

	Knight* knight = xnew<Knight>(100);

	xdelete(knight);

	knight->_hp = 100;
}
*/

// 스마트 포인터
/*
using KnightRef = TSharedPtr<class Knight>;
using InventoryRef = TSharedPtr<class Inventory>;

class Knight : public RefCountable
{
public:
	Knight()
	{
		cout << "Knight()" << endl;
	}

	~Knight()
	{
		cout << "~Knight()" << endl;
	}

	void SetTarget(KnightRef target)
	{
		_target = target;
	}

	KnightRef _target = nullptr;
	InventoryRef _inventory = nullptr;
};

class Inventory : public RefCountable
{
public:
	Inventory(KnightRef knight) : _knight(**knight) //레퍼런스 카운터를 증가시키지 않고 참조
	{

	}

	Knight& _knight;
};

int main()
{
	// 1) 이미 만들어진 클래스 대상으로 사용 불가
	// 2) 순환 (Cycle) 문제

	KnightRef k1(new Knight());
	k1->ReleaseRef();
	// 거의 일어나지 않지만, 순환 문제 발생
	//KnightRef k2(new Knight());
	//k2->ReleaseRef();

	//k1->SetTarget(k2);
	//k2->SetTarget(k1);

	//k1 = nullptr;
	//k2 = nullptr;

	k1->_inventory = new Inventory(k1);
	//결과: 레퍼런스 카운트: k1 - 1 / inventory - 2

	//shared_ptr<Knight> spr(new Knight());
	shared_ptr<Knight> spr = make_shared<Knight>();
	weak_ptr<Knight> wpr = spr;

	bool expired = wpr.expired();
	shared_ptr<Knight> spr2 = wpr.lock();
	if (spr2 != nullptr)
	{

	}

}
*/

//Reference Counting
/*
class Wraight;
class Missile;

using WraightRef = TSharedPtr<Wraight>;
using MissileRef = TSharedPtr<Missile>;

class Wraight : public RefCountable
{
public:
	int _hp = 150;
	int _poxX = 0;
	int _poxY = 0;
};

class Missile : public RefCountable
{
public:
	void SetTarget(WraightRef target)
	{
		_target = target;
		// 중간에 개입 가능
		//target->AddRef();
		Test(target);
	}

	//레퍼런스를 넘길 때 비용이 들 수 있으므로 스마트포인터 레퍼런스로 받아옴
	void Test(WraightRef& target)
	{

	}

	bool Update()
	{
		if (_target == nullptr)
			return true;

		int posX = _target->_poxX; 
		//_target이 delete 된 후에도, 그 공간을 계속 참조하여 연산함
		int posY = _target->_poxY;

		// TODO : 쫓아간다

		if (_target->_hp == 0)
		{
			//_target->ReleaseRef();
			_target = nullptr;
			return true;
		}

		return false;
	}

	WraightRef _target = nullptr;
};

int main()
{
	//Wraight* wraight = new Wraight();
	//Missile* missile = new Missile();
	WraightRef wraight(new Wraight());
	wraight->ReleaseRef(); // 레퍼런스 카운트가 처음 생성될 때 2가 되므로 1로 만듦
	MissileRef missile(new Missile());
	missile->ReleaseRef();

	missile->SetTarget(wraight);
	

	//레이스가 피격 당함
	wraight->_hp = 0;
	// delete wraight;
	// wraight->ReleaseRef();
	wraight = nullptr; // 복사 연산자 실행


	while (true)
	{
		if (missile)
		{
			missile->Update();
		}
	}

	//delete missile;
	// missile->ReleaseRef();
	missile = nullptr;
}
*/

// 소수 구하기
/*
#include <vector>

bool IsPrime(int number)
{
	if (number <= 1)
		return false;
	if (number == 2 || number == 3)
		return true;

	for (int i = 2; i < number; i++)
	{
		if ((number % i) == 0)
			return false;
	}

	return true;
}

// [start - end]
int CountPrime(int start, int end)
{
	int count = 0;
	
	for (int number = start; number <= end; number++)
	{
		if (IsPrime(number))
			count++;
	}

	return count;
}

// 1과 자기 자신으로만 나뉘면 그것을 소수라고 함

int main()
{
	const int MAX_NUMBER = 100'0000;
	// 1-MAX_NUMBER까지의 소수 개수

	vector<thread> threads;

	int coreCount = thread::hardware_concurrency();

	cout << "CoreCount: " << coreCount << endl;

	int jobCount = (MAX_NUMBER / coreCount) + 1;

	atomic<int> primeCount = 0;

	for (int i = 0; i < coreCount; i++)
	{
		int start = (i * jobCount) + 1;
		int end = min(MAX_NUMBER, ((i + 1) * jobCount));

		threads.push_back(thread([start, end, &primeCount]()
			{
				primeCount += CountPrime(start, end);
			}));
	}

	for (thread& t : threads)
		t.join();

	cout << primeCount << endl;
}
*/

// DeadLock Profiler
/*
#include "PlayerManager.h"
#include "LAccountManager.h"

int main()
{

	GThreadManager->Launch([=]
		{
			while (true)
			{
				cout << "PlayerThenAccount" << endl;
				GPlayerManager.PlayerThenAccount();
				this_thread::sleep_for(100ms);
			}
		});

	GThreadManager->Launch([=]
		{
			while (true)
			{
				cout << "AccountThenPlayer" << endl;
				GAccountManager.AccountThenPlayer();
				this_thread::sleep_for(100ms);
			}
		});

	GThreadManager->Join();
}
*/
//RW Lock
/*
class TestLock
{
	USE_LOCK;

public:
	int32 TestRead()
	{
		READ_LOCK;

		if (_queue.empty())
			return -1;

		return _queue.front();
	}

	void TestPush()
	{
		WRITE_LOCK;

		_queue.push(rand() % 100);
	}

	void TestPop()
	{
		WRITE_LOCK;

		if (_queue.empty() == false)
			_queue.pop();
	}

private:
	queue<int32> _queue;
};

TestLock testLock;

void ThreadWrite()
{
	while (true)
	{
		testLock.TestPush();
		this_thread::sleep_for(1ms);
		testLock.TestPop();
	}
}

void ThreadRead()
{
	while (true)
	{
		int32 value = testLock.TestRead();
		cout << value << endl;
		this_thread::sleep_for(1ms);
	}
}

int main()
{
	for (int32 i = 0; i < 2; i++)
	{
		GThreadManager->Launch(ThreadWrite);
	}
	for (int32 i = 0; i < 5; i++)
	{
		GThreadManager->Launch(ThreadRead);
	}

	GThreadManager->Join();
}

*/

//Lock-Based Stack/Queue
/*
#include "ConcurrentQueue.h"
#include "ConcurrentStack.h"

LockQueue<int32> q;
LockStack<int32> s;
LockFreeStack<int32> fs;
LockFreeSmartStack<int32> ss;
LockFreeQueue<int32> fq;

void Push()
{
	while (true)
	{
		int32 value = rand() % 100;
		//q.Push(value);
		//fs.Push(value);
		//ss.Push(value);
		fq.Push(value);

		this_thread::sleep_for(10ms);
	}
}

void Pop()
{
	while (true)
	{
		int32 data = 0;
		
		//if(q.TryPop(OUT data))
		//	cout << data << endl;

		//if (fs.TryPop(OUT data))
		//	cout << data << endl;
		//auto smartData = ss.TryPop();
		//if (smartData != nullptr)
		//	cout << (*smartData) << endl;
		auto smartData = fq.TryPop();
		if (smartData != nullptr)
			cout << (*smartData) << endl;
	}
}

int main()
{
	thread t1(Push);
	thread t2(Pop);

	t1.join();
	t2.join();
}
*/

//Thread Local Storage
/*
//__declspec(thread) int32 value;
thread_local int32 LThreadId = 0;
//thread_local queue<int32> q;

void ThreadMain(int32 threadId)
{
	LThreadId = threadId;

	while (true)
	{
		cout << "Hi! I am Thread " << &LThreadId << endl;
		this_thread::sleep_for(1s);
	}
}

int main()
{
	vector<thread> threads;

	for (int32 i = 0; i < 10; i++)
	{
		int32 threadId = i + 1;
		threads.push_back(thread(ThreadMain, threadId));
	}

	for (thread& t : threads)
		t.join();
}
*/
/*
int main()
{
	atomic<bool> flag = false;

	bool lockFree = flag.is_lock_free();
	//0: lock 필요함
	//1: 이미 원자적으로 처리하므로 lock 필요없음


}
*/
/*

int32 x = 0;
int32 y = 0;
int32 r1 = 0;
int32 r2 = 0;
volatile bool ready;

void Thread_1()
{
	while (!ready)
		;
	y = 1; //Store y
	r1 = x; //Load x
}

void Thread_2()
{
	while (!ready)
		;
	x = 1; //Store x
	r2 = y; //Load y
}

int main()
{
	int32 count = 0;
	
	while (true)
	{
		ready = false;
		count++;

		x = y = r1 = r2 = 0;

		thread t1(Thread_1);
		thread t2(Thread_2);

		ready = true;

		t1.join();
		t2.join();

		if (r1 == 0 && r2 == 0)
			break;
	}

	cout << count << " 번만에 빠져나옴ㅎㅎ" << endl;
}
*/
/*
int32 buffer[10000][10000];

int main()
{
	memset(buffer, 0, sizeof(buffer));

	// 빠름
	{
		uint64 start = GetTickCount64();

		int64 sum = 0;
		for (int32 i = 0; i < 10000; i++)
			for (int32 j = 0; j < 10000; j++)
				sum += buffer[i][j]; //일련의 데이터에서 한칸씩 이동

		uint64 end = GetTickCount64();

		cout << "Elapsed Tick" << (end - start) << endl;

	}

	// 더 느림
	{
		uint64 start = GetTickCount64();

		int64 sum = 0;
		for (int32 i = 0; i < 10000; i++)
			for (int32 j = 0; j < 10000; j++)
				sum += buffer[j][i];

		uint64 end = GetTickCount64();

		cout << "Elapsed Tick" << (end - start) << endl;

	}

	// 데이터 하나를 불러올 때, 인접데이터도 함께 캐시에 등록 (special locality)
	// 캐시에 있는 데이터를 불러올 때, 캐시 히트가 되며, 캐시 히트가 되면 메모리에서까지 데이터를 불러올 필요 없음
}

*/


/*

int64 result;

int64 Calculate()
{
	int64 sum = 0;

	for (int32 i = 0; i < 100'000; i++)
		sum += 1;
	
	return sum;
}

void PromiseWorker(std::promise<string>&& promise)
{
	promise.set_value("Secret Messege");
}

void TaskWorker(std::packaged_task<int64(void)>&& task)
{
	task();
}
int main()
{
	//동기(synchronous) 실행
	int64 sum = Calculate();
	cout << sum << endl;

	//비동기 != 멀티 쓰레드

	// std::future
	// Cacluate 라는 일감 하나를 실행하기 위한 전용 쓰레드를 만들어 떠넘긴 상태
	{
		// 인수 속성
		// 1) defferred -> lazy evaluation 지연 실행
		// 2) async -> 별도의 쓰레드를 만들어서 실행
		// 3) deffered | async -> 둘 중 알아서 선택
		// 비동기 방식으로 Calculate 함수 호출, 언젠가 미래에 결과물을 반환 
		std::future<int64> future = std::async(std::launch::async, Calculate);

		// TODO
		//std::future_status  status = future.wait_for(1ms);
		//if (status == future_status::ready)
		//{

		//}

		//future.wait(); //wait 후에 get 호출하나 wait도 결과물이 나올때까지 기다린다.

		int64 sum = future.get(); //결과물이 이제서야 필요하다

		//class Knight
		//{
		//public:
		//	int64 GetHp() { return 100; }
		//};
		//Knight knight;
		//std::future<int64> future2 = std::async(std::launch::async, &Knight::GetHp, knight);//클래스 메소드 호출 예제
	}

	// std::promise

	{
		// 미래(std::future)에 결과물을 반환해줄 것이라고 약속함 (std::promise) 
		std::promise<string> promise; //promise는 메인 쓰레드가 가짐
		std::future<string> future = promise.get_future();

		thread t(PromiseWorker, std::move(promise)); //PromiseWorker 쓰레드에 promise를 넘김

		string message = future.get();
		cout << message << endl;

		t.join();
	}

	// std::packaged_task
	// 쓰레드를 만들어 일감을 넘김
	// 한 쓰레드가 여러 테스크를 처리할 수 있음
	{
		std::packaged_task<int64(void)> task(Calculate); //Calcuate 결과물 자체가 future를 통해 받아올 수 있음
		std::future<int64> future = task.get_future();

		std::thread t(TaskWorker, std::move(task)); //thread가 호출될 때 task가 호출이 되고 future 객체를 통해 추출해줄 수 있음

		int64 sum = future.get();
		cout << sum << endl;

		t.join();
	}

	// 결론)
	// mutex, condition_variable까지 가지 않고 단순한 애들을 처리할 수 있는
	// 특히나, 한 번 발생하는 이벤트에 유용하다!
	// 닭 잡는데 소 잡는 칼 쓸 필요 없다!
	// 1) async
	// 원하는 함수를 비동기적으로 실행
	// 2) promise
	// 결과물을 promise를 통해 future로 받아줌
	// 3) packaged_task
	// 원하는 함수의 실행 결과를 packaged_task를 통해 future로 받아줌
}
*/

/*
mutex m;
queue<int32> q;
HANDLE handle;

// 참고) CV 는 User-Level Object (커널 오브젝트X)
condition_variable cv;

void Producer()
{
	while (true)
	{
		// 1) Lock을 잡고
		// 2) 공유 변수 값을 수정
		// 3) Lock을 풀고
		// 4) 조건변수 통해 다른 쓰레드에게 통지
		{
			unique_lock<mutex> lock(m);
			q.push(100);
		}

		cv.notify_one(); // wait 중인 쓰레드가 있으면 딱 1개를 깨운다

		//Event Lock
		//::SetEvent(handle); //커널 오브젝트 handle을 singal 상태로 변환
		//this_thread::sleep_for(10000ms);
	}
}

void Consumer()
{
	int32 i = 0;
	while (true)
	{
		unique_lock<mutex> lock(m);
		cv.wait(lock, []() { return q.empty() == false; }); //조건
		// 1) Lock 잡고
		// 2) 조건 확인
		// - 만족0 => 빠져 나와서 이어서 코드 진행
		// - 만족X => Lock 풀어주고 대기 상태

		// 그런데 notify_one을 했으면 항상 조건식을 만족하는가?
		// Suprious Wakeup (가짜 기상?)
		// notify_one할 때 lock을 잡고 있는 것이 아니기 때문

		// Event Lock
		//::WaitForSingleObject(handle, INFINITE); //논 시그널 상태일 시 수면상태 //bManualRset이 자동리셋이므로 Signal일 시 다시 Non-Signal로
		////::ResetEvent(handle); //자동리셋 아닐 시 수동으로 Non-Signal로 리셋하여야 함

		//unique_lock<mutex> lock(m);
		while (q.empty() == false)
		{
			int32 data = q.front();
			q.pop();
			cout << data << " " << i++ << endl;
		}
	}
}

int main()
{
	// 커널 오브젝트
	// Usage Count
	// Signal (파란불) / Non-Signal (빨간불) << bool
	// Auto / Manual << bool
	//window api, bManualReset: 수동 리셋/자동 리셋(false), bInitialState: signal/non signal
	handle = ::CreateEvent(
		NULL, //보안속성
		FALSE,//bManualReset
		FALSE,//bInitialState
		NULL
	); //handel 인덱스를 넘김

	thread t1(Producer);
	thread t2(Consumer);

	t1.join();
	t2.join();

	::CloseHandle(handle);
}
*/

/*
class SpinLock
{
public:
	void lock()
	{
		//CAS (Compare-And-Swap)
		
		bool expected = false;
		bool desired = true;

		//// CAS 의사 코드
		//if (_locked == expected)
		//{
		//	expected = _locked;
		//	_locked = desired;
		//	return true;
		//}
		//else
		//{
		//	expected = _locked;
		//	return false;
		//}
		
		//epxrected: locked 값을 예상해주고, desired: locked값이 바뀌길 원하는 값
		while (_locked.compare_exchange_strong(expected, desired) == false) //expected에 _locked 값이 들어가고 expected가 desired 값이 되면 성공 true 반환
		{
			expected = false;

			this_thread::sleep_for(0ms);
			//this_thread::yield(); <--this_thread::sleep_for(0ms);
			//this_thread::sleep_for(std::chrono::milliseconds(100));
		}

		//while (_locked)
		//{

		//}
		//_locked = true;
	}

	void unlock()
	{
		//_locked = false;
		_locked.store(false); //_locked 변수가 atomic임을 알림
	}

private:
	//volatile: 컴파일러에게 최적화 X
	//volatile bool _locked = false;

	atomic<bool> _locked = false;
};

int32 sum = 0;
mutex m;
SpinLock spinLock;

void Add()
{
	for (int32 i = 0; i < 10'0000; i++)
	{
		lock_guard<SpinLock> guard(spinLock);
		sum++;
	}
}

void Sub()
{
	for (int32 i = 0; i < 10'0000; i++)
	{
		lock_guard<SpinLock> guard(spinLock);
		sum--;
	}
}

int main()
{
	thread t1(Add);
	thread t2(Sub);

	t1.join();
	t2.join();

	cout << sum << endl;
}
*/

/*
#include "AccountManager.h"
#include "UserManager.h"

void Func1()
{
	for (int32 i = 0; i < 100; i++)
	{
		UserManager::Instance()->ProcessSave();
	}
}

void Func2()
{
	for (int32 i = 0; i < 100; i++)
	{
		AccountManager::Instance()->ProcessLogin();
	}
}

int main()
{
	std::thread t1(Func1);
	std::thread t2(Func2);

	t1.join();
	t2.join();

	cout << "Jobs Done" << endl;

	////참고
	//mutex m1;
	//mutex m2;
	//std::lock(m1, m2); 내부적으로 mutex의 일련의 번호를 발급하여 deadlock 관리

	//// adopt_lock: 이미 lock된 상태니까, 나중에 소멸될 때 lock을 풀도록 함
	//lock_guard<mutex> g1(m1, std::adopt_lock);
	//lock_guard<mutex> g1(m2, std::adopt_lock);
}

*/
/*

vector<int32> v; //atomic 사용불가

// Mutual Exclusive (상호배타적)
mutex m;

// RAII (Resource Acquisition is Initialization)
template<typename T>
class LockGuard
{
public:
	LockGuard(T& m)
	{
		_mutex = &m;
		_mutex->lock();
	}

	~LockGuard() //지역변수로 활용된 클래스가 더이상 활용되지 않을 때 자동으로 폐기, 언락
	{
		_mutex->unlock();
	}

private:
	T* _mutex;

};

void Push()
{
	for (int32 i = 0; i < 10000; i++)
	{
		//자물쇠 잠그기
		//m.lock();
		//LockGuard<std::mutex> lockGuard(m);
		std::lock_guard<std::mutex> lockGuard(m);

		//std::unique_lock<std::mutex> uniqueLock(m, std::defer_lock); //바로 잠그지 않고, 잠그는 시점을 따로 설정이 가능
		//uniqueLock.lock();
		

		v.push_back(i);

		if (i == 5000)
			break;

		//m.unlock();
	}
}

int main()
{
	//v.reserve(20000);

	std::thread t1(Push);
	std::thread t2(Push);

	t1.join();
	t2.join();

	cout << v.size() << endl;
}
*/

/*

// atomic : All-Or-Nothing

// DB
atomic<int32> sum = 0;

void Add()
{
	for (int32 i = 0; i < 100'0000; i++)
	{
		sum.fetch_add(1);
		//sum++;
		//int32 eax = sum;
		//eax = eax + 1;
		//sum = eax;
	}
}

void Sub()
{
	for (int32 i = 0; i < 100'0000; i++)
	{
		sum.fetch_add(-1);
		//sum--;
		//int32 eax = sum;
		//eax = eax - 1;
		//sum = eax;
	}
}

int main()
{
	Add();
	Sub();

	cout << sum << endl;

	std::thread t1(Add);
	std::thread t2(Sub);
	t1.join();
	t2.join();
	cout << sum << endl;
}

*/

/*

void HelloThread()
{
	cout << "Hello Thread" << endl;
}

void HelloThread_2(int32 num)
{
	cout << num << endl;
}

int main()
{
	vector<std::thread> v;

	for (int32 i = 0; i < 10; i++)
	{
		v.push_back(std::thread(HelloThread_2, i));
	}

	//std::thread t(HelloThread_2, 10);
	//
	//if(t.joinable())
	//	t.join();

	for (int32 i = 0; i < 10; i++)
	{
		if (v[i].joinable())
			v[i].join();
	}

	cout << "Hello Main" << endl;
}

*/