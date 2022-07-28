#pragma once

/*-----------------
	IocpObject
-----------------*/

class IocpObject : public enable_shared_from_this<IocpObject>
{
	// enable_shared_from_this<IocpObject>�� ����� ��
	//weak_ptr<IocpObject> _Wptr;
public:
	virtual HANDLE GetHandle() abstract;
	// � �ϰ����� iocpEvent�� �޾ƿ�
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

	// Iocp�� ����ϴ� �Լ�
	bool Register(class IocpObject* iocpObject);
	// ��Ŀ ��������� Iocp�� �ϰ��� �ִ��� �˻�
	bool Dispatch(uint32 timeoutMs = INFINITE);

private:
	HANDLE _iocpHandle;
};

// TEMP
extern IocpCore GIocpCore;

