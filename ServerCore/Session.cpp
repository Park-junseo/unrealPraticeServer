#include "pch.h"
#include "Session.h"
#include "SocketUtils.h"

/*-------------
	Session
-------------*/

Session::Session()
{
	_socket = SocketUtils::CreateSocket();
}

Session::~Session()
{
	SocketUtils::Close(_socket);
}

void Session::DisConnect(const WCHAR* cause)
{
}

HANDLE Session::GetHandle()
{
	return reinterpret_cast<HANDLE>(_socket);
}

void Session::Dispatch(IocpEvent* iocpEvent, int32 numOfBytes)
{
	// TODO
}

void Session::RegisterConnect()
{
}

void Session::RegisterRecv()
{
	if (IsConnected() == false)
		return;
	
	////매번 새롭게 등록
	//RecvEvent* recvEvent = xnew<RecvEvent>();
	//recvEvent->owner = shared_from_this();
	
	// 어차피 생성하므로 멤버변수로 재사용
	_recvEvent.owner = shared_from_this(); // ADD_REF, Register 하는 동안 섹션일 살아있게 하기 위함

	WSABUF wsaBuf;
	wsaBuf.buf = reinterpret_cast<char*>(_recvBuffer);
	wsaBuf.len = len32(_recvBuffer);

	DWORD numOfBytes = 0;
	DWORD flags = 0;
	if (SOCKET_ERROR == ::WSARecv(_socket, &wsaBuf, 1, OUT & numOfBytes, OUT & flags, &_recvEvent, nullptr))
	{
		int32 errorCode = ::WSAGetLastError();
		if (errorCode != WSA_IO_PENDING)
		{
			HandleError(errorCode);
			_recvEvent.owner = nullptr; // Release_REF
		}
	}
}

void Session::RegisterSend()
{
}

void Session::ProcessConnect()
{
}

void Session::ProcessRecv(int32 numOfBytes)
{
}

void Session::ProcessSend(int32 numOfBytes)
{
}

void Session::HandleError(int32 erroCode)
{
}
