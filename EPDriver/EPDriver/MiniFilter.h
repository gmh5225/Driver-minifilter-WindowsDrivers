#pragma once
#include <stdafx.h>

struct MINIFITLER_CALLBACKS {
    PFLT_FILTER_UNLOAD_CALLBACK FilterUnload;
};

class MiniFilter
{
public:
    static NTSTATUS Init(PDRIVER_OBJECT DriverObject);
    static void Uninit();

    static bool Register(PVOID Key, MINIFITLER_CALLBACKS Callbacks);
    static void Unregister(PVOID Key);
};


