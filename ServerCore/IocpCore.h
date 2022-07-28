#pragma once

/*-----------------
	IocpObject
-----------------*/

class IocpObject : public enable_shared_from_this<IocpObject>
{
	// enable_shared_from_this<IocpObject>를 상속할 시
	//weak_ptr<IocpObject> _Wptr;
public:
	virtual HANDLE GetHandle() abstract;
	// 어떤 일감인지 iocpEvent로 받아옴
	virtual void Dispatch(class IocpEvent* iocpEvent, int32 numOfBytes = 0) abstract;
};

/*---------------
	IocpCore
---------------*/

class IocpCore
{
public:
	IocpCore();
	~IocpCore();

	HANDLE GetHandle() { return _iocpHandle; }

	// Iocp에 등록하는 함수
	bool Register(class IocpObject* iocpObject);
	// 워커 쓰레드들이 Iocp에 일감이 있는지 검색
	bool Dispatch(uint32 timeoutMs = INFINITE);

private:
	HANDLE _iocpHandle;
};

// TEMP
extern IocpCore GIocpCore;

