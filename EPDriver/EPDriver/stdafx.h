#pragma once

#include <fltKernel.h>
#include <ntddk.h>

void* __cdecl operator new(size_t buffer);
void __cdecl operator delete(void* address, size_t size);

#ifdef _DEBUG
#define DbgLog(...) DbgPrint(__VA_ARGS__)
#else
#define DbgLog(...);
#endif

#define InfLog(...) DbgPrint(__VA_ARGS__)