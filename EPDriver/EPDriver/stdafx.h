#pragma once

#include <fltKernel.h>
#include <ntddk.h>

void* __cdecl operator new(size_t buffer);
void __cdecl operator delete(void* address, size_t size);