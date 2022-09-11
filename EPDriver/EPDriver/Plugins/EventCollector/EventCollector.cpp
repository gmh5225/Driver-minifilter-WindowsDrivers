#include <stdafx.h>

#include "EventCollector.h"

NTSTATUS EventCollector::Init(_In_ PDRIVER_OBJECT DriverObject, _In_ PUNICODE_STRING RegistryPath)
{
	DbgPrint("%s(%p, %p)\n", __FUNCTION__, DriverObject, RegistryPath);
	return STATUS_SUCCESS;
}
NTSTATUS EventCollector::Start()
{
	DbgPrint("%s()\n", __FUNCTION__);
	return STATUS_SUCCESS;
}
void EventCollector::Stop()
{
	DbgPrint("%s()\n", __FUNCTION__);
}
void EventCollector::Uninit()
{
	DbgPrint("%s()\n", __FUNCTION__);
}
