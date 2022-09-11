#pragma once
#include <stdafx.h>

class ProcessFilter
{
public:
	static NTSTATUS Init(_In_ PDRIVER_OBJECT DriverObject, _In_ PUNICODE_STRING RegistryPath);
	static NTSTATUS Start();
	static void Stop();
	static void Uninit();

    struct Registration {
        PCREATE_PROCESS_NOTIFY_ROUTINE_EX CreateProcessNotifyRoutineEx = NULL;
    };
    static bool Register(PVOID Key, Registration &Callbacks);
    static void Unregister(PVOID Key);
};