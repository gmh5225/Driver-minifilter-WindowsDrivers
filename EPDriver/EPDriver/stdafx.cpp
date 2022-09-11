#include "stdafx.h"

void* __cdecl operator new(size_t size)
{
    return ExAllocatePool2(POOL_FLAG_NON_PAGED, size, 'ppc0');
}

void __cdecl operator delete(void* address, size_t size)
{
	size;
	ExFreePool(address);
}