#include <stdafx.h>
#include "ProcessFilter.h"

#include <Utils/Collections/List.h>
#include <Utils/Locks/EresourceLock.h>

struct REGISTERED_CALLBACKS { PVOID Key;  PROCESSFILTER_CALLBACKS Callbacks; };
static List<REGISTERED_CALLBACKS>* RegisteredCallbacks = NULL;
static EresourceLock* RegisteredCallbacksLock = NULL;

void CreateProcessNotifyRoutineEx(
    _Inout_     PEPROCESS Process,
    _In_        HANDLE ProcessId,
    _Inout_opt_ PPS_CREATE_NOTIFY_INFO CreateInfo
)
{
    DbgLog("%s(%p, %d, %p)\n", __FUNCTION__, Process, ProcessId, CreateInfo);
    RegisteredCallbacksLock->AcquireShared();
    for (auto& registered : *RegisteredCallbacks)
        if (registered.Callbacks.CreateProcessNotifyRoutineEx)
            registered.Callbacks.CreateProcessNotifyRoutineEx(Process, ProcessId, CreateInfo);
    RegisteredCallbacksLock->Release();
}

// ProcessFilter

NTSTATUS ProcessFilter::Init(_In_ PDRIVER_OBJECT DriverObject, _In_ PUNICODE_STRING RegistryPath)
{
    UNREFERENCED_PARAMETER(DriverObject);
    UNREFERENCED_PARAMETER(RegistryPath);
    DbgLog("%s()\n", __FUNCTION__);
    if (
        (RegisteredCallbacks = new List<REGISTERED_CALLBACKS>()) == NULL ||
        (RegisteredCallbacksLock = new EresourceLock()) == NULL)
        goto CLEAN_UP;
    return STATUS_SUCCESS;

CLEAN_UP:
    if (RegisteredCallbacks)
        delete RegisteredCallbacks;
    if (RegisteredCallbacksLock)
        delete RegisteredCallbacksLock;
    return STATUS_UNSUCCESSFUL;
}
NTSTATUS ProcessFilter::Start()
{
	DbgLog("%s()\n", __FUNCTION__);
    NTSTATUS ret = STATUS_SUCCESS;
    bool process_create = false;

    // Setup ProcessCreate routine
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
void ProcessFilter::Stop()
{
	DbgLog("%s()\n", __FUNCTION__);
    PsSetCreateProcessNotifyRoutineEx(CreateProcessNotifyRoutineEx, TRUE);
}
void ProcessFilter::Uninit()
{
	DbgLog("%s()\n", __FUNCTION__);
    if (RegisteredCallbacks)
        delete RegisteredCallbacks;
    if (RegisteredCallbacksLock)
        delete RegisteredCallbacksLock;
}

bool ProcessFilter::Register(PVOID Key, PROCESSFILTER_CALLBACKS &Callbacks)
{
    RegisteredCallbacksLock->AcquireExclusive();
    bool found = false;
    bool ret = false;
    for (auto& registered : *RegisteredCallbacks)
        if (registered.Key == Key)
        {
            found = true;
            break;
        }
    if (!found)
        ret = RegisteredCallbacks->push_back(REGISTERED_CALLBACKS{ Key, Callbacks });
    RegisteredCallbacksLock->Release();
    return ret;

}
void ProcessFilter::Unregister(PVOID Key)
{
    RegisteredCallbacksLock->AcquireExclusive();
    auto found = RegisteredCallbacks->end();
    for (auto ite = RegisteredCallbacks->begin(); ite != RegisteredCallbacks->end(); ite++)
        if (ite->Key == Key)
            found = ite;
    if (found != RegisteredCallbacks->end())
        RegisteredCallbacks->erase(found);
    RegisteredCallbacksLock->Release();
}
