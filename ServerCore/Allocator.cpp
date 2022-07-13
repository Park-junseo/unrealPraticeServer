#include "pch.h"
#include "Allocator.h"

/********************
	BaseAllocator
********************/

void* BaseAllocator::Alloc(int32 size)
{
	return ::malloc(size); //사이즈 할당
}

void BaseAllocator::Release(void* ptr)
{
	::free(ptr);
}

/********************
	StompAllocator
********************/

void* StompAllocator::Alloc(int32 size)
{
	const int64 pageCount = (size + PAGE_SIZE - 1) / PAGE_SIZE;
	const int64 dataOffset = pageCount * PAGE_SIZE - size;
	void* baseAddress = ::VirtualAlloc(NULL, pageCount * PAGE_SIZE, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
	//메모릴를 할당해놓고, 할당된 메모리의 끝부분에 필요한 데이터의 크기만 남겨놓고 시작위치를 가르킴
	return static_cast<void*>(static_cast<int8*>(baseAddress) + dataOffset);
}

void StompAllocator::Release(void* ptr)
{
	const int64 address = reinterpret_cast<int64>(ptr);
	const int64 baseAddress = address - (address % PAGE_SIZE);
	::VirtualFree(reinterpret_cast<void*>(baseAddress), 0, MEM_RELEASE);
}
