#include <stdafx.h>
#include "EventCollectorProcess.h"
#include <Filters/ProcessFilter.h>

static void* IdKey;

static void CreateProcessNotifyRoutineEx(
    _Inout_     PEPROCESS Process,
    _In_        HANDLE ProcessId,
    _Inout_opt_ PPS_CREATE_NOTIFY_INFO CreateInfo
)
{
    UNREFERENCED_PARAMETER(Process);
    UNREFERENCED_PARAMETER(ProcessId);
    UNREFERENCED_PARAMETER(CreateInfo);
    InfLog("%s(%p, %llu, %p)\n", __FUNCTION__, Process, (ULONG_PTR) ProcessId, CreateInfo);
}

NTSTATUS EventCollectorProcess::Start()
{
    DbgLog("%s()\n", __FUNCTION__);

    // Make callbacks registration
    ProcessFilter::Registration callbacks = {};
    callbacks.CreateProcessNotifyRoutineEx = CreateProcessNotifyRoutineEx;

    // Regsiter to ProcessFilter
    if (!ProcessFilter::Register(&IdKey, callbacks))
    {
        InfLog("%s() -> error: Register\n", __FUNCTION__);
        return STATUS_UNSUCCESSFUL;
    }
    return STATUS_SUCCESS;
}
void EventCollectorProcess::Stop()
{
    DbgLog("%s()\n", __FUNCTION__);
    ProcessFilter::Unregister(&IdKey);
}