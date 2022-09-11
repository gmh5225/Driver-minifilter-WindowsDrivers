#pragma once
#include <stdafx.h>

class MiniFilter
{
public:
    static NTSTATUS Init(PDRIVER_OBJECT DriverObject);
    static void Uninit();

    struct Registration {
        PFLT_FILTER_UNLOAD_CALLBACK FilterUnload = NULL;
    };
    static bool Register(PVOID Key, Registration &Callbacks);
    static void Unregister(PVOID Key);
};


