#pragma once
#include <stdafx.h>

class ProcessFilter
{
public:
	static NTSTATUS Init(_In_ PDRIVER_OBJECT DriverObject, _In_ PUNICODE_STRING RegistryPath);
	static NTSTATUS Start();
	static void Stop();
	static void Uninit();
};