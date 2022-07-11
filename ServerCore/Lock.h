#pragma once

#include "Types.h"

/****************************
	Reader-Writer SpinLock
****************************/

/*
* 32bit 변수를 활용, 상위 16bit와 하위 16bit가 용도가 다름
[\\\\\\\\][\\\\\\\\][RRRRRRRR][RRRRRRRR]
\ : WriteFlag (Excusive Lock Owner ThreadId)
R : ReadFlag (Shared Lock Count)
*/

class Lock
{

	enum : uint32
	{
		ACQUIRE_TIMEOUT_TICK = 10000,
		MAX_SPIN_COUNT = 5000,
		WRITE_THREAD_MASK = 0xFFFF'0000,
		READ_COUNT_MASK = 0x0000'FFFF,
		EMPTY_FLAG = 0x0000'0000
	};

public:
	void WriteLock();
	void WriteUnlock();
	void ReadLock();
	void ReadUnlock();

private:

	Atomic<uint32> _lockFlag = EMPTY_FLAG;

	//writeLock의 재귀를 허용하여 writeCount 값을 관리
	uint16 _writeCount = 0;
};

/*****************
	LockGuards
*****************/

class ReadLockGuard
{
public:
	ReadLockGuard(Lock& lock) : _lock(lock) { _lock.ReadLock(); }
	~ReadLockGuard() { _lock.ReadUnlock(); }

private:
	Lock& _lock;
};

class WriteLockGuard
{
public:
	WriteLockGuard(Lock& lock) : _lock(lock) { _lock.WriteLock(); }
	~WriteLockGuard() { _lock.WriteUnlock(); }

private:
	Lock& _lock;
};