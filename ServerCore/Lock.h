#pragma once

#include "Types.h"

/****************************
	Reader-Writer SpinLock
****************************/

/*
* 32bit ������ Ȱ��, ���� 16bit�� ���� 16bit�� �뵵�� �ٸ�
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
	uint16 _writeCount = 0;
};

