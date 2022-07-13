#include "pch.h"
#include "Allocator.h"

void* BaseAllocator::Alloc(int32 size)
{
	return ::malloc(size); //������ �Ҵ�
}

void BaseAllocator::Release(void* ptr)
{
	::free(ptr);
}
