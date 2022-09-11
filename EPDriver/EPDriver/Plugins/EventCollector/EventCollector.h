#pragma once
#include <stdafx.h>
#include "Events/Events.h"

class EventCollector
{
public:
	static NTSTATUS Init(_In_ PDRIVER_OBJECT DriverObject, _In_ PUNICODE_STRING RegistryPath);
	static NTSTATUS Start();
	static void Stop();
	static void Uninit();

    // Give ownership of event memory to EventCollector
    static void OnEvent(Event *event);
};