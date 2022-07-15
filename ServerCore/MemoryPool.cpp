#include "pch.h"
#include "MemoryPool.h"

/*******************
	MemoryPool
*******************/

MemoryPool::MemoryPool(int32 allocSize) : _allocSize(allocSize)
{
	::InitializeSListHead(&_header);
}

MemoryPool::~MemoryPool()
{
	

	//while (_queue.empty() == false)
	//{
	//	MemoryHeader* header = _queue.front();
	//	_queue.pop();
	//	::free(header);
	//}

	while (MemoryHeader* memory = static_cast<MemoryHeader*>(::InterlockedPopEntrySList(&_header)))
	{
		::_aligned_free(memory);
	}
}

void MemoryPool::Push(MemoryHeader* ptr)
{
	//WRITE_LOCK;
	ptr->allocSize = 0;

	// Pool에 메모리 반납
	//_queue.push(ptr);
	::InterlockedPushEntrySList(&_header, static_cast<PSLIST_ENTRY>(ptr));

	_allocCount.fetch_add(1);
}

MemoryHeader* MemoryPool::Pop()
{
	//MemoryHeader* header = nullptr;

	//{
	//	WRITE_LOCK;
	//	// Pool에 여분이 있는지?
	//	if (_queue.empty() == false)
	//	{
	//		// 있으면 하나 꺼내온다
	//		header = _queue.front();
	//		_queue.pop();
	//	}
	//}

	MemoryHeader* memory = static_cast<MemoryHeader*>(::InterlockedPopEntrySList(&_header));

	// 없으면 새로 만든다
	if (memory == nullptr)
	{
		//header = reinterpret_cast<MemoryHeader*>(::malloc(_allocSize));
		memory = reinterpret_cast<MemoryHeader*>(::_aligned_malloc(_allocSize,SLIST_ALIGNMENT));
	}
	else
	{
		ASSERT_CRASH(memory->allocSize == 0);
	}

	_allocCount.fetch_add(1);

	return memory;
}
