#pragma once

class Session;

enum class EventType : uint8
{
	Connect,
	Accept,
	//PreRecv,
	Recv,
	Send
};

/*-----------------
	IocpEvent
-----------------*/

class IocpEvent : public OVERLAPPED
{
public:
	IocpEvent(EventType type);

	void			Init();
	//EventType	GetType() { return eventType;  }

public:
	EventType		eventType;
	IocpObjectRef	owner;
};

/*-----------------
	ConnectEvent
-----------------*/
class ConnectEvent : public IocpEvent
{
public:
	ConnectEvent() : IocpEvent(EventType::Connect) { }
};

/*-----------------
	AcceptEvent
-----------------*/
class AcceptEvent : public IocpEvent
{
public:
	AcceptEvent() : IocpEvent(EventType::Accept) { }

	//void		SetSession(Session* session) { _session = session; }
	//Session*	GetSession() { return _session; }

public:
	SessionRef	session = nullptr;
};

/*-----------------
	RecvEvent
-----------------*/
class RecvEvent : public IocpEvent
{
public:
	RecvEvent() : IocpEvent(EventType::Recv) { }
};

/*-----------------
	SendEvent
-----------------*/
class SendEvent : public IocpEvent
{
public:
	SendEvent() : IocpEvent(EventType::Send) { }

	// TEMP
	vector<BYTE> buffer;
};