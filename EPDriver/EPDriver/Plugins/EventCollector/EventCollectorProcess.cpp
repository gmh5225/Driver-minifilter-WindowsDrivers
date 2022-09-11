#include <stdafx.h>
#include "EventCollectorProcess.h"

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
    NTSTATUS ret = STATUS_SUCCESS;
    bool process_create = false;

    // Set ProcessCreate routine
    {
        ret = PsSetCreateProcessNotifyRoutineEx(CreateProcessNotifyRoutineEx, FALSE);
        if (ret != STATUS_SUCCESS)
        {
            InfLog("%s() -> error: 0x%X, at %s:%d\n", __FUNCTION__, ret, __FILE__, __LINE__);
            goto CLEAN_UP;
        }
        process_create = true;
    }

    return STATUS_SUCCESS;

CLEAN_UP:
    if (process_create)
        PsSetCreateProcessNotifyRoutineEx(CreateProcessNotifyRoutineEx, TRUE);
    return ret;
}
void EventCollectorProcess::Stop()
{
    PsSetCreateProcessNotifyRoutineEx(CreateProcessNotifyRoutineEx, TRUE);
}