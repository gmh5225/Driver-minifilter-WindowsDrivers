#include <stdafx.h>
#include "MiniFilter.h"

#include <Utils/Collections/List.h>
#include <Utils/Locks/EresourceLock.h>

NTSTATUS FilterUnload(FLT_FILTER_UNLOAD_FLAGS Flags);

// Variables

static PFLT_FILTER Filter = NULL;

static List<PFLT_PORT> *CommunicationPorts = NULL;
static EresourceLock* CommunicationPortsLock = NULL;

struct REGISTERED_CALLBACKS { PVOID Key = NULL;  MiniFilter::Registration Callbacks = {}; };
static List<REGISTERED_CALLBACKS>* RegisteredCallbacks = NULL;
static EresourceLock* RegisteredCallbacksLock = NULL;

static const FLT_OPERATION_REGISTRATION OperationRegistration[] = {
    //{},
    {IRP_MJ_OPERATION_END}
};

static const FLT_REGISTRATION FilterRegistration = {
    sizeof(FLT_REGISTRATION),
    FLT_REGISTRATION_VERSION,
    NULL,                               // Flags
    NULL,                               // ContextRegistration,
    OperationRegistration,              // OperationRegistration
    FilterUnload,                       // FilterUnloadCallback
    NULL,                               // InstanceSetupCallback
    NULL,                               // InstanceQueryTeardownCallback
    NULL,                               // InstanceTeardownStartCallback
    NULL,                               // InstanceTeardownCompleteCallback
    NULL,                               // GenerateFileNameCallback
    NULL,                               // NormalizeNameComponentCallback
    NULL,                               // NormalizeContextCleanupCallback
    NULL,                               // TransactionNotificationCallback
    NULL,                               // NormalizeNameComponentExCallback
    NULL,                               // SectionNotificationCallback
};

NTSTATUS FilterUnload(FLT_FILTER_UNLOAD_FLAGS Flags)
{
    DbgLog("%s()\n", __FUNCTION__);
    NTSTATUS ret = STATUS_SUCCESS;

    // Call Callbacks
    RegisteredCallbacksLock->AcquireShared();
    for (auto& registered : *RegisteredCallbacks)
        if (registered.Callbacks.FilterUnload)
        {
            ret = registered.Callbacks.FilterUnload(Flags);
            if (ret != STATUS_SUCCESS)
            {
                InfLog("%s() Key: %p -> 0x%X\n", __FUNCTION__, registered.Key, ret);
                break;
            }
        }
    RegisteredCallbacksLock->Release();
    if (ret != STATUS_SUCCESS)
        return ret;

    // Cleanup MiniFilter 
    CommunicationPortsLock->AcquireExclusive();
    for (auto& port : *CommunicationPorts)
        FltCloseCommunicationPort(port);
    return STATUS_SUCCESS;
}

// MiniFilter

NTSTATUS MiniFilter::Init(PDRIVER_OBJECT DriverObject)
{
    DbgLog("%s()\n", __FUNCTION__);
    if (
        (CommunicationPorts = new List<PFLT_PORT>()) == NULL ||
        (CommunicationPortsLock = new EresourceLock()) == NULL || 
        (RegisteredCallbacks = new List<REGISTERED_CALLBACKS>()) == NULL ||
        (RegisteredCallbacksLock = new EresourceLock()) == NULL)
    {
        InfLog("%s() -> error: 0x%X, at %s:%d\n", __FUNCTION__, STATUS_MEMORY_NOT_ALLOCATED, __FILE__, __LINE__);
        goto CLEAN_UP;
    }

    NTSTATUS ret = FltRegisterFilter(DriverObject, &FilterRegistration, &Filter);
    if (ret != STATUS_SUCCESS)
    {
        InfLog("%s() -> error: 0x%X, at %s:%d\n", __FUNCTION__, ret, __FILE__, __LINE__);
        goto CLEAN_UP;
    }
      
    return STATUS_SUCCESS;

CLEAN_UP:
    if (CommunicationPorts)
        delete CommunicationPorts;
    if (CommunicationPortsLock)
        delete CommunicationPortsLock;
    if (RegisteredCallbacks)
        delete RegisteredCallbacks;
    if (RegisteredCallbacksLock)
        delete RegisteredCallbacksLock;
    return STATUS_UNSUCCESSFUL;
}

void MiniFilter::Uninit()
{
    DbgLog("%s()\n", __FUNCTION__);

    FltUnregisterFilter(Filter);

    if (CommunicationPorts)
        delete CommunicationPorts;
    if (CommunicationPortsLock)
        delete CommunicationPortsLock;
    if (RegisteredCallbacks)
        delete RegisteredCallbacks;
    if (RegisteredCallbacksLock)
        delete RegisteredCallbacksLock;
}

bool MiniFilter::Register(PVOID Key, Registration &Callbacks)
{
    DbgLog("%s()\n", __FUNCTION__);

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

void MiniFilter::Unregister(PVOID Key)
{
    DbgLog("%s()\n", __FUNCTION__);

    RegisteredCallbacksLock->AcquireExclusive();
    auto found = RegisteredCallbacks->end();
    for (auto ite = RegisteredCallbacks->begin(); ite != RegisteredCallbacks->end(); ite++)
        if (ite->Key == Key)
            found = ite;
    if (found != RegisteredCallbacks->end())
        RegisteredCallbacks->erase(found);
    RegisteredCallbacksLock->Release();
}