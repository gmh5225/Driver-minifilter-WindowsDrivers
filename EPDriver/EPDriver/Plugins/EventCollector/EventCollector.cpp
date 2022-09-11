#include <stdafx.h>

#include "EventCollector.h"
#include "EventCollectorProcess.h"

#include <Utils/Collections/List.h>
#include <Utils/Locks/EresourceLock.h>

class _EventQueue
{
private:
    List<Event*> list;
    EresourceLock lock;
public:
    _EventQueue() {}
    ~_EventQueue()
    {
        lock.AcquireExclusive();
        for (auto event : list)
            delete event;
        list.clear();
        lock.Release();
    }

    bool push(Event* event)
    {
        lock.AcquireExclusive();
        bool ret = list.push_back(event);
        lock.Release();
        return ret;
    }

    Event* pop()
    {
        lock.AcquireExclusive();
        Event* ret = nullptr;
        if (!list.empty())
        {
            ret = *list.begin();
            list.erase(list.begin());
        }
        lock.Release();
        return ret;
    }

};
static _EventQueue *EventQueue = NULL;


NTSTATUS EventCollector::Init(_In_ PDRIVER_OBJECT DriverObject, _In_ PUNICODE_STRING RegistryPath)
{
    UNREFERENCED_PARAMETER(DriverObject);
    UNREFERENCED_PARAMETER(RegistryPath);
	DbgLog("%s()\n", __FUNCTION__);
    if (
        (EventQueue = new _EventQueue()) == NULL)
    {
        InfLog("%s() -> error: 0x%X, at %s:%d\n", __FUNCTION__, STATUS_MEMORY_NOT_ALLOCATED, __FILE__, __LINE__);
        goto CLEAN_UP;
    }

	return STATUS_SUCCESS;

CLEAN_UP:
    if (EventQueue)
        delete EventQueue;
    return STATUS_UNSUCCESSFUL;
}
NTSTATUS EventCollector::Start()
{
	DbgLog("%s()\n", __FUNCTION__);
    NTSTATUS ret = STATUS_SUCCESS;
    bool process = false;
    
    // Process
    {
        ret = EventCollectorProcess::Start();
        if (ret != STATUS_SUCCESS)
        {
            InfLog("%s() -> error: 0x%X, at %s:%d\n", __FUNCTION__, ret, __FILE__, __LINE__);
            goto CLEAN_UP;
        }
        process = true;
    }

	return STATUS_SUCCESS;

CLEAN_UP:
    if (process)
        EventCollectorProcess::Stop();
    return ret;
}
void EventCollector::Stop()
{
	DbgLog("%s()\n", __FUNCTION__);
    EventCollectorProcess::Stop();
}
void EventCollector::Uninit()
{
	DbgLog("%s()\n", __FUNCTION__);
    if (EventQueue)
        delete EventQueue;
}

void EventCollector::OnEvent(Event* event)
{
    if (!EventQueue->push(event))
        delete event;
}