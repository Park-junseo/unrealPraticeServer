#include "pch.h"
#include "Lock.h"
#include "CoreTLS.h"

void Lock::WriteLock()
{
	//�ƹ��� ������ �� �����ϰ� ���� ���� ��, �����ؼ� �������� ��´�

	const uint32 desired = ((LThreadId << 16) & WRITE_THREAD_MASK);
	while (true)
	{
		for (uint32 spinCount = 0; spinCount < MAX_SPIN_COUNT; spinCount++)
		{
			uint32 expected = EMPTY_FLAG;
			if (_lockFlag.compare_exchange_strong(OUT expected, desired))
			{

			}
		}
	}
}

void Lock::WriteUnlock()
{
}

void Lock::ReadLock()
{
}

void Lock::ReadUnlock()
{
}
