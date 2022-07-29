#include "pch.h"
#include "Listener.h"
#include "SocketUtils.h"
#include "IocpEvent.h"
#include "Session.h"
#include "Service.h"

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

bool Listener::StartAccept(ServerServiceRef service)
{
	_service = service;
	if (_service == nullptr)
		return false;

	_socket = SocketUtils::CreateSocket();
	if (_socket == INVALID_SOCKET)
		return false;

	// listen ���ϵ� ��������̹Ƿ� ����ؾ� ��
	if (_service->GetIocpCore()->Register(shared_from_this()) == false)
		return false;

	if (SocketUtils::SetReuseAddress(_socket, true) == false)
		return false;

	if (SocketUtils::SetLinger(_socket, 0, 0) == false)
		return this;

	if (SocketUtils::Bind(_socket, _service->GetNetAddress()) == false)
		return false;

	if (SocketUtils::Listen(_socket) == false)
		return false;

	const int32 acceptCount = _service->GetmaxSessionCount();
	for (int32 i = 0; i < acceptCount; i++)
	{
		AcceptEvent* acceptEvent = xnew<AcceptEvent>();
		//(X) acceptEvent->owner = shared_ptr<IocpObject>(this); <-- refCounter�� ������� �ʰ� ���Ӱ� ������
		acceptEvent->owner = shared_from_this(); // �ڱ� �ڽſ� ���� refCounter�� ������ ä�� shared_ptr ����
		_acceptEvents.push_back(acceptEvent);
		RegisterAccept(acceptEvent);
	} 
	return true;
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

// accept�� �ɾ�Z iocp���� ó���� �� �ֵ��� �ϰ��� ȣ���Ͽ� ����
void Listener::RegisterAccept(AcceptEvent* acceptEvent)
{
	//Session* session = xnew<Session>();
	SessionRef session = _service->CreateSession(); // Register IOCP

	acceptEvent->Init();
	//acceptEvent->SetSession(session);
	acceptEvent->session = session;

	DWORD bytesReceived = 0;
	// �񵿱� accept ȣ��
	if (false == SocketUtils::AcceptEx(_socket, session->GetSocket(), session->_recvBuffer, 0, sizeof(SOCKADDR_IN) + 16, sizeof(SOCKADDR_IN) + 16, OUT & bytesReceived, static_cast<LPOVERLAPPED>(acceptEvent)))
	{
		const int32 errorCode = ::WSAGetLastError();
		if (errorCode != WSA_IO_PENDING)
		{
			// �ϴ� �ٽ� Accept �ɾ��ش�
			RegisterAccept(acceptEvent);
		}
	}
}

// ó�� ������ accpetEvent�� ��� ������ �� ����
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

	session->SetNetAddress(NetAddress(sockAddress));
	session->ProcessConnect();
	/*�������ʿ��� Ŀ������ �ξ����� 
	�������ʿ��� ���� �� �Լ��� ȣ��(OnConnected())
	Listener���� RegisterAccept�� �񵿱⸦ �ɾ��ְ� ���������� ó���ϸ�, ProcessAccept ȣ��*/
	RegisterAccept(acceptEvent);
}
