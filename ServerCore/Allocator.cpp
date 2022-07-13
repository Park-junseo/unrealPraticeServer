#include "pch.h"
#include "Allocator.h"

void* BaseAllocator::Alloc(int32 size)
{
	return ::malloc(size); //사이즈 할당
}

void BaseAllocator::Release(void* ptr)
{
	::free(ptr);
}
