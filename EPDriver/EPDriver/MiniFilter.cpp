#include <stdafx.h>
#include "MiniFilter.h"

NTSTATUS FilterUnload(FLT_FILTER_UNLOAD_FLAGS Flags);

static PFLT_FILTER Filter = NULL;

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
    UNREFERENCED_PARAMETER(Flags);
    return STATUS_SUCCESS;
}

NTSTATUS MiniFilterRegister(PDRIVER_OBJECT DriverObject)
{
    return FltRegisterFilter(
        DriverObject,
        &FilterRegistration,
        &Filter
    );
}
void MiniFilterUnregister()
{
    FltUnregisterFilter(Filter);
}
