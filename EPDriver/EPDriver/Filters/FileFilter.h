#pragma once
#include <stdafx.h>
#include <MiniFilter.h>

class FileFilter
{
public:
    static NTSTATUS Init(_In_ PDRIVER_OBJECT DriverObject, _In_ PUNICODE_STRING RegistryPath);
    static NTSTATUS Start();
    static void Stop();
    static void Uninit();

    struct Registration {

    };
    static bool Register(PVOID Key, Registration& Callbacks);
    static void Unregister(PVOID Key);
};