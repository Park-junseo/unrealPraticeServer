#include "pch.h"
#include "Lock.h"
#include "CoreTLS.h"

void Lock::WriteLock()
{
	// 1) ������ �����尡 �����ϰ� �ִٸ� ������ ����
	// ���� lockFlag���� WriteFlag ���� ������
	const uint32 lockThreadId = (_lockFlag.load() & WRITE_THREAD_MASK) >> 16;
	if (LThreadId == lockThreadId)
	{
		_writeCount++;
		return;
	}

	// 2) �ƹ��� ������ �� �����ϰ� ���� ���� ��, �����ؼ� �������� ��´�
	const int64 beginTick = ::GetTickCount64();
	// LThreadId�� WRITE_THREAD_MASK�� WriteFlag Bit�� ����
	const uint32 desired = ((LThreadId << 16) & WRITE_THREAD_MASK);
	while (true)
	{
		for (uint32 spinCount = 0; spinCount < MAX_SPIN_COUNT; spinCount++)
		{
			uint32 expected = EMPTY_FLAG;
			// _lockFlag�� EMPTY_FLAG�� Read Lock�� ��� ���� �� ThreadId�� Lock�� ���� �� ����
			if (_lockFlag.compare_exchange_strong(OUT expected, desired))
			{
				//�����尡 WriteFlag�� ȹ���� ����
				_writeCount++;
				return;
			}
		}

		// �ð��� �ʹ� ��ü�� �� CRASH
		if (::GetTickCount64() - beginTick >= ACQUIRE_TIMEOUT_TICK)
			CRASH("LOCK_TIMEOUT");

		// MAX_SPIN_COUNT�� �ѱ� �������� ���� ��
		this_thread::yield();
	}
}

void Lock::WriteUnlock()
{
	// ReadLock �� Ǯ�� ������ WriteUnlock �Ұ���
	if ((_lockFlag.load() & READ_COUNT_MASK) != 0)
		CRASH("INVALID_UNLOCK_ORDER");

	const int32 lockCount = --_writeCount;
	// write �ϴ� �����尡 ���� �ڽ� ���̹Ƿ� EMPTY_FLAG ������
	if (lockCount == 0)
		_lockFlag.store(EMPTY_FLAG);
}

void Lock::ReadLock()
{
	// 1) ������ �����尡 �����ϰ� �ִٸ� ������ ����
	const uint32 lockThreadId = (_lockFlag.load() & WRITE_THREAD_MASK) >> 16;
	if (LThreadId == lockThreadId)
	{
		_lockFlag.fetch_add(1);
		return;
	}

	// 2) �ƹ��� �����ϰ� ���� ���� �� �����ؼ� ���� ī��Ʈ�� �ø���
	const int64 beginTick = ::GetTickCount64();
	while (true)
	{
		for (uint32 spinCount = 0; spinCount < MAX_SPIN_COUNT; spinCount++)
		{
			uint32 expected = (_lockFlag.load() & READ_COUNT_MASK);
			// expected �� WriteFlag�� ��� �ִ� �����̰ų� ������ ReadFlag�� ������ ���
			if (_lockFlag.compare_exchange_strong(OUT expected, expected + 1))
				return;
		}

		// �ð��� �ʹ� ��ü�� �� CRASH
		if (::GetTickCount64() - beginTick >= ACQUIRE_TIMEOUT_TICK)
			CRASH("LOCK_TIMEOUT");

		this_thread::yield();
	}
}

void Lock::ReadUnlock()
{
	//ReadLock�� �ϱ� ���� ReadUnlock�� �� ���
	if ((_lockFlag.fetch_sub(1) & READ_COUNT_MASK) == 0)
		CRASH("MULTIPLE_UNLOCK");
}
