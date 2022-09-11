#pragma once
#include <stdafx.h>
#include "EventCollector.h"

class EventCollectorProcess
{
public:
    static NTSTATUS Start();
    static void Stop();
};