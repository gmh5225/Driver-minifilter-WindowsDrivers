#include <stdafx.h>
#include "MiniFilter.h"

#include <Utils/Collections/List.h>
#include <Utils/Locks/EresourceLock.h>

NTSTATUS FilterUnload(FLT_FILTER_UNLOAD_FLAGS Flags);

// Variables

static PFLT_FILTER Filter = NULL;

static List<PFLT_PORT> *CommunicationPorts = NULL;
static EresourceLock* CommunicationPortsLock = NULL;

struct REGISTERED_CALLBACKS { PVOID Key;  MINIFITLER_CALLBACKS Callbacks; };
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
    NTSTATUS ret = STATUS_SUCCESS;

    // Call Callbacks
    RegisteredCallbacksLock->AcquireShared();
    for (auto& registered : *RegisteredCallbacks)
        if (registered.Callbacks.FilterUnload)
        {
            ret = registered.Callbacks.FilterUnload(Flags);
            if (ret != STATUS_SUCCESS)
                break;
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
    if (
        (CommunicationPorts = new List<PFLT_PORT>()) == NULL ||
        (CommunicationPortsLock = new EresourceLock()) == NULL || 
        (RegisteredCallbacks = new List<REGISTERED_CALLBACKS>()) == NULL ||
        (RegisteredCallbacksLock = new EresourceLock()) == NULL)
        goto CLEAN_UP;

    if (FltRegisterFilter(DriverObject, &FilterRegistration, &Filter) == STATUS_SUCCESS)
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

bool MiniFilter::Register(PVOID Key, MINIFITLER_CALLBACKS Callbacks)
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

void MiniFilter::Unregister(PVOID Key)
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