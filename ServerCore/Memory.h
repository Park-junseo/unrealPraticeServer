#pragma once
#include "Allocator.h"

template<typename Type, typename... Args>
Type* xnew(Args&&... args)
{
	Type* memory = static_cast<Type*>(x_alloc(sizeof(Type)));

	// placement new, 할당된 메모리 위에다 생성
	new(memory) Type(std::forward<Args>(args)...);
	return memory;  
}

template<typename Type>
void xdelete(Type* obj)
{
	obj->~Type();
	x_release(obj);
}