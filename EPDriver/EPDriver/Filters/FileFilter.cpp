#include <stdafx.h>
#include "FileFilter.h"
#include <MiniFilter.h>

#include <Utils/Collections/List.h>
#include <Utils/Locks/EresourceLock.h>

static void* IdKey;

struct REGISTERED_CALLBACKS { PVOID Key = NULL;  FileFilter::Registration Callbacks = {}; };
static List<REGISTERED_CALLBACKS>* RegisteredCallbacks = NULL;
static EresourceLock* RegisteredCallbacksLock = NULL;

// FileFilter

NTSTATUS FileFilter::Init(_In_ PDRIVER_OBJECT DriverObject, _In_ PUNICODE_STRING RegistryPath)
{
    UNREFERENCED_PARAMETER(DriverObject);
    UNREFERENCED_PARAMETER(RegistryPath);
    DbgLog("%s()\n", __FUNCTION__);
    if (
        (RegisteredCallbacks = new List<REGISTERED_CALLBACKS>()) == NULL ||
        (RegisteredCallbacksLock = new EresourceLock()) == NULL)
    {
        InfLog("%s() -> error: 0x%X, at %s:%d\n", __FUNCTION__, STATUS_MEMORY_NOT_ALLOCATED, __FILE__, __LINE__);
        goto CLEAN_UP;
    }
    return STATUS_SUCCESS;

CLEAN_UP:
    if (RegisteredCallbacks)
        delete RegisteredCallbacks;
    if (RegisteredCallbacksLock)
        delete RegisteredCallbacksLock;
    return STATUS_UNSUCCESSFUL;
}
NTSTATUS FileFilter::Start()
{
    DbgLog("%s()\n", __FUNCTION__);

    // Make callbacks registration
    MiniFilter::Registration callbacks = {};

    // Regsiter to Minifilter
    if (!MiniFilter::Register(&IdKey, callbacks))
    {
        InfLog("%s() -> error: Register\n", __FUNCTION__);
        return STATUS_UNSUCCESSFUL;
    }
    return STATUS_SUCCESS;
}
void FileFilter::Stop()
{
    DbgLog("%s()\n", __FUNCTION__);
    MiniFilter::Unregister(&IdKey);
}
void FileFilter::Uninit()
{
    DbgLog("%s()\n", __FUNCTION__);
    if (RegisteredCallbacks)
        delete RegisteredCallbacks;
    if (RegisteredCallbacksLock)
        delete RegisteredCallbacksLock;
}

bool FileFilter::Register(PVOID Key, Registration& Callbacks)
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
void FileFilter::Unregister(PVOID Key)
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
