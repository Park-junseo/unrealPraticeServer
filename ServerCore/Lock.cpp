#include "pch.h"
#include "Lock.h"
#include "CoreTLS.h"

void Lock::WriteLock()
{
	// 1) 동일한 쓰레드가 소유하고 있다면 무조건 성공
	// 현재 lockFlag에서 WriteFlag 값을 가져옴
	const uint32 lockThreadId = (_lockFlag.load() & WRITE_THREAD_MASK) >> 16;
	if (LThreadId == lockThreadId)
	{
		_writeCount++;
		return;
	}

	// 2) 아무도 소유권 및 공유하고 있지 않을 때, 경합해서 소유권을 얻는다
	const int64 beginTick = ::GetTickCount64();
	// LThreadId이 WRITE_THREAD_MASK로 WriteFlag Bit를 차지
	const uint32 desired = ((LThreadId << 16) & WRITE_THREAD_MASK);
	while (true)
	{
		for (uint32 spinCount = 0; spinCount < MAX_SPIN_COUNT; spinCount++)
		{
			uint32 expected = EMPTY_FLAG;
			// _lockFlag가 EMPTY_FLAG인 Read Lock이 비어 있을 때 ThreadId로 Lock을 가질 수 있음
			if (_lockFlag.compare_exchange_strong(OUT expected, desired))
			{
				//쓰레드가 WriteFlag를 획득한 상태
				_writeCount++;
				return;
			}
		}

		// 시간이 너무 지체될 시 CRASH
		if (::GetTickCount64() - beginTick >= ACQUIRE_TIMEOUT_TICK)
			CRASH("LOCK_TIMEOUT");

		// MAX_SPIN_COUNT를 넘긴 다음에는 한턴 쉼
		this_thread::yield();
	}
}

void Lock::WriteUnlock()
{
	// ReadLock 다 풀기 전에는 WriteUnlock 불가능
	if ((_lockFlag.load() & READ_COUNT_MASK) != 0)
		CRASH("INVALID_UNLOCK_ORDER");

	const int32 lockCount = --_writeCount;
	// write 하는 쓰레드가 오직 자신 뿐이므로 EMPTY_FLAG 값으로
	if (lockCount == 0)
		_lockFlag.store(EMPTY_FLAG);
}

void Lock::ReadLock()
{
	// 1) 동일한 쓰레드가 소유하고 있다면 무조건 성공
	const uint32 lockThreadId = (_lockFlag.load() & WRITE_THREAD_MASK) >> 16;
	if (LThreadId == lockThreadId)
	{
		_lockFlag.fetch_add(1);
		return;
	}

	// 2) 아무도 소유하고 있지 않을 때 경합해서 공유 카운트를 올린다
	const int64 beginTick = ::GetTickCount64();
	while (true)
	{
		for (uint32 spinCount = 0; spinCount < MAX_SPIN_COUNT; spinCount++)
		{
			uint32 expected = (_lockFlag.load() & READ_COUNT_MASK);
			// expected 가 WriteFlag가 비어 있는 상태이거나 누군가 ReadFlag를 차지할 경우
			if (_lockFlag.compare_exchange_strong(OUT expected, expected + 1))
				return;
		}

		// 시간이 너무 지체될 시 CRASH
		if (::GetTickCount64() - beginTick >= ACQUIRE_TIMEOUT_TICK)
			CRASH("LOCK_TIMEOUT");

		this_thread::yield();
	}
}

void Lock::ReadUnlock()
{
	//ReadLock을 하기 전에 ReadUnlock을 한 경우
	if ((_lockFlag.fetch_sub(1) & READ_COUNT_MASK) == 0)
		CRASH("MULTIPLE_UNLOCK");
}
