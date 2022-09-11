#include "stdafx.h"

void* __cdecl operator new(size_t size)
{
	auto address = ExAllocatePool2(POOL_FLAG_NON_PAGED, size, 'ppc0');
	DbgPrint("%s() -> %p\n", __FUNCTION__, address);
	return address;
}

void __cdecl operator delete(void* address, size_t size)
{
	size;
	ExFreePool(address);
	DbgPrint("%s() -> %p\n", __FUNCTION__, address);
}