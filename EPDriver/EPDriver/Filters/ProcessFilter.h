#pragma once
#include <stdafx.h>

struct PROCESSFILTER_CALLBACKS {
    PCREATE_PROCESS_NOTIFY_ROUTINE_EX CreateProcessNotifyRoutineEx = NULL;
};

class ProcessFilter
{
public:
	static NTSTATUS Init(_In_ PDRIVER_OBJECT DriverObject, _In_ PUNICODE_STRING RegistryPath);
	static NTSTATUS Start();
	static void Stop();
	static void Uninit();

    static bool Register(PVOID Key, PROCESSFILTER_CALLBACKS &Callbacks);
    static void Unregister(PVOID Key);
};