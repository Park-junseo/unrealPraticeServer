#pragma once
#include "IocpCore.h"
#include "IocpEvent.h"
#include "NetAddress.h"
#include "RecvBuffer.h"

class Service;

/*-------------
	Session
-------------*/

class Session : public IocpObject
{
	friend class Listener;
	friend class IocpCore;
	friend class Service;

	enum
	{
		BUFFER_SIZE = 0x10000, //64KB
	};
public:
	Session();
	virtual ~Session();

public:
						/* �ܺο��� ��� */
	//void				Send(BYTE* buffer, int32 len);
	void				Send(SendBufferRef sendBuffer);
	bool				Connect(); // ��� ������ ����
	void				Disconnect(const WCHAR* cause); // Disconnect�� �� ������ ���

	shared_ptr<Service>	GetService() { return _service.lock(); } // service�� shared_ptr�� ��ȯ
	void				SetService(shared_ptr<Service> service) { _service = service; } // �ܺο��� ���� �����Լ�

public:
						/* ���� ���� */
	void				SetNetAddress(NetAddress address) { _netAddress = address; }
	NetAddress			GetAddress() { return _netAddress; }
	SOCKET				GetSocket() { return _socket;  }
	bool				IsConnected() { return _connected; }
	SessionRef			GetSessionRef() { return static_pointer_cast<Session>(shared_from_this()); }

private:
						/* �������̽� ���� */
	virtual HANDLE		GetHandle() override;
	virtual void		Dispatch(class IocpEvent* iocpEvent, int32 numOfBytes = 0) override;

private:
						/* ���� ���� */
	bool				RegisterConnect();
	bool				RegisterDisconnect();
	void				RegisterRecv();
	//void				RegisterSend(SendEvent* sendEvent);
	void				RegisterSend();
	
	void				ProcessConnect();
	void				ProcessDisconnect();
	void				ProcessRecv(int32 numOfBytes);
	//void				ProcessSend(SendEvent* sendEvent, int32 numOfBytes);
	void				ProcessSend(int32 numOfBytes);

	void				HandleError(int32 errorCode);
	/*
	RegisterConnect()->ProcessConnect()
	RegisterRecv()->ProcessRecv(int32)
	RegisterSend()->ProcessSend(int32)
	Register�� �񵿱� �ɰ� Process �Լ� ȣ��

	OnConnected() : �������� �� �α׸� ����� üũ�� �� ���Ǹ��� �������̵��ؼ� ���
	OnRecv() : ���� ���۷� ó��
	OnSend() : �����͸� ���������� ������ �� ȣ��
	OnDisConnected() : ������ �� ȣ��*/

protected:
						/* ������ �ڵ忡�� ������ */
	virtual void		OnConnected() {}
	virtual int32		OnRecv(BYTE* buffer, int32 len) { return len; }
	virtual void		OnSend(int32 len) { }
	virtual void		OnDisconnected() { }

//public:
//	// TEMP
//	BYTE _recvBuffer[1000];
//
//	// ���� ������ �����ϱ� ���� ��� ���� ����� �� ����
//	// Circular buffer�� ���� ������ �̾���̰� ���κ� ���� �� �������� ó�� �κк��� �̾� �ٿ��� �ϴµ�,
//	//  �̴� ���� ����� ���� �Ҹ��
//	// Circular Buffer [       ]
//	//char _sendBuffer[1000];
//	//int32 _sendLen = 0;

private:
	weak_ptr<Service>	_service;
	/*���������� ���񽺿� ���� ���縦 �˾ƾ� ���������� ���񽺸� ����ϰų� ���� �� �����Ƿ� weak_ptr�� ������ �Ѵ�.
	������ ������� �ʴ� �̻�, ��� ����ǹǷ� weak_ptr�� ��� �ְ�, null üũ���� �ʰ� �ٷ� ����ϰ� �Ѵ�.*/
	SOCKET				_socket = INVALID_SOCKET;
	NetAddress			_netAddress = {};
	Atomic<bool>		_connected = false;

private:
	USE_LOCK;

							/* ���� ���� */
	RecvBuffer				_recvBuffer;

							/* �۽� ���� */
	xQeque<SendBufferRef>	_sendQueue;
	Atomic<bool>			_sendRegistered = false;

private:
						/* IocpEvent ���� */
	ConnectEvent		_connectEvent;
	DisconnectEvent		_disconnectEvent;
	RecvEvent			_recvEvent;
	SendEvent			_sendEvent;
};

