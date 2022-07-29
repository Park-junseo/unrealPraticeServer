#pragma once
#include "IocpCore.h"
#include "IocpEvent.h"
#include "NetAddress.h"

class Service;

/*-------------
	Session
-------------*/

class Session : public IocpObject
{
	friend class Listener;
	friend class IocpCore;
	friend class Service;
public:
	Session();
	virtual ~Session();

public:
	void				DisConnect(const WCHAR* cause); // Disconnect할 때 사유도 출력

	shared_ptr<Service>	GetService() { return _service.lock(); } // service로 shared_ptr로 반환
	void				SetService(shared_ptr<Service> service) { _service = service; } // 외부에서 서비스 지정함수

public:
						/* 정보 관련 */
	void				SetNetAddress(NetAddress address) { _netAddress = address; }
	NetAddress			GetAddress() { return _netAddress; }
	SOCKET				GetSocket() { return _socket;  }
	bool				IsConnected() { return _connected; }
	SessionRef			GetSessionRef() { return static_pointer_cast<Session>(shared_from_this()); }

private:
						/* 인터페이스 구현 */
	virtual HANDLE		GetHandle() override;
	virtual void		Dispatch(class IocpEvent* iocpEvent, int32 numOfBytes = 0) override;

private:
						/* 전송 관련 */
	void				RegisterConnect();
	void				RegisterRecv();
	void				RegisterSend();
	
	void				ProcessConnect();
	void				ProcessRecv(int32 numOfBytes);
	void				ProcessSend(int32 numOfBytes);

	void				HandleError(int32 erroCode);
	/*
	RegisterConnect()->ProcessConnect()
	RegisterRecv()->ProcessRecv(int32)
	RegisterSend()->ProcessSend(int32)
	Register로 비동기 걸고 Process 함수 호출

	OnConnected() : 연결했을 때 로그를 띄워나 체크할 때 섹션마다 오버라이드해서 사용
	OnRecv() : 받은 버퍼로 처리
	OnSend() : 데이터를 성공적으로 보냈을 때 호출
	OnDisConnected() : 끊겼을 때 호출*/

protected:
						/* 컨텐츠 코드에서 오버로딩 */
	virtual void		OnConnected() {}
	virtual int32		OnRecv(BYTE* buffer, int32 len) { return len; }
	virtual void		OnSend(int32 len) { }
	virtual void		OnDisconnected() { }

public:
	// TEMP
	char _recvBuffer[1000];

private:
	weak_ptr<Service>	_service;
	/*내부적으로 서비스에 대한 존재를 알아야 실질적으로 서비스를 등록하거나 끊을 수 있으므로 weak_ptr를 가지게 한다.
	서버가 종료되지 않는 이상, 계속 실행되므로 weak_ptr로 들고 있고, null 체크하지 않고 바로 사용하게 한다.*/
	SOCKET				_socket = INVALID_SOCKET;
	NetAddress			_netAddress = {};
	Atomic<bool>		_connected = false;

private:
	USE_LOCK;

	/* 수신 관련 */

	/* 송신 관련 */

private:
						/* IocpEvent 재사용 */
	RecvEvent			_recvEvent;
};

