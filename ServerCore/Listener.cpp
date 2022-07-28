#include "pch.h"
#include "Listener.h"
#include "SocketUtils.h"
#include "IocpEvent.h"
#include "Session.h"

/*--------------
	Listener
---------------*/

Listener::~Listener()
{
	SocketUtils::Close(_socket);

	for (AcceptEvent* acceptEvent : _acceptEvents)
	{
		// TODO

		xdelete(acceptEvent);
	}
}

bool Listener::StartAccept(NetAddress netAddress)
{
	_socket = SocketUtils::CreateSocket();
	if (_socket == INVALID_SOCKET)
		return false;

	// listen 소켓도 관찰대상이므로 등록해야 함
	if (GIocpCore.Register(this) == false)
		return false;

	if (SocketUtils::SetReuseAddress(_socket, true) == false)
		return false;

	if (SocketUtils::SetLinger(_socket, 0, 0) == false)
		return this;

	if (SocketUtils::Bind(_socket, netAddress) == false)
		return false;

	if (SocketUtils::Listen(_socket) == false)
		return false;

	const int32 acceptCount = 1;
	for (int32 i = 0; i < acceptCount; i++)
	{
		AcceptEvent* acceptEvent = xnew<AcceptEvent>();
		//(X) acceptEvent->owner = shared_ptr<IocpObject>(this); <-- refCounter를 고려하지 않고 새롭게 생성함
		acceptEvent->owner = shared_from_this(); // 자기 자신에 대한 refCounter를 유지한 채로 shared_ptr 생성
		_acceptEvents.push_back(acceptEvent);
		RegisterAccept(acceptEvent);
	} 
	return false;
}

void Listener::CloseSocket()
{
	SocketUtils::Close(_socket);
}

HANDLE Listener::GetHandle()
{
	return reinterpret_cast<HANDLE>(_socket);
}

void Listener::Dispatch(IocpEvent* iocpEvent, int32 numOfBytes)
{
	//ASSERT_CRASH(iocpEvent->GetType() == EventType::Accept);
	ASSERT_CRASH(iocpEvent->eventType == EventType::Accept);

	AcceptEvent* acceptEvent = static_cast<AcceptEvent*>(iocpEvent);
	ProcessAccept(acceptEvent);
}

// accept를 걸어줫 iocp에서 처리할 수 있도록 일감을 호출하여 예약
void Listener::RegisterAccept(AcceptEvent* acceptEvent)
{
	//Session* session = xnew<Session>();
	SessionRef session = MakeShared<Session>();

	acceptEvent->Init();
	//acceptEvent->SetSession(session);
	acceptEvent->session = session;

	DWORD bytesReceived = 0;
	// 비동기 accept 호출
	if (false == SocketUtils::AcceptEx(_socket, session->GetSocket(), session->_recvBuffer, 0, sizeof(SOCKADDR_IN) + 16, sizeof(SOCKADDR_IN) + 16, OUT & bytesReceived, static_cast<LPOVERLAPPED>(acceptEvent)))
	{
		const int32 errorCode = ::WSAGetLastError();
		if (errorCode != WSA_IO_PENDING)
		{
			// 일단 다시 Accept 걸어준다
			RegisterAccept(acceptEvent);
		}
	}
}

// 처음 생성된 accpetEvent를 계속 재사용할 수 있음
void Listener::ProcessAccept(AcceptEvent* acceptEvent)
{
	//Session* session = acceptEvent->GetSession();
	SessionRef session = acceptEvent->session;

	if (false == SocketUtils::SetUpdateAcceptSocket(session->GetSocket(), _socket))
	{
		RegisterAccept(acceptEvent);
		return;
	}

	SOCKADDR_IN sockAddress;
	int32 sizeOfSockAddr = sizeof(sockAddress);
	if (SOCKET_ERROR == ::getpeername(session->GetSocket(), OUT reinterpret_cast<SOCKADDR*>(&sockAddress), &sizeOfSockAddr))
	{
		RegisterAccept(acceptEvent);
		return;
	}

	session->SetNetAddress(NetAddress(sockAddress));

	cout << "Client Connected" << endl;

	// TODO

	RegisterAccept(acceptEvent);
}
