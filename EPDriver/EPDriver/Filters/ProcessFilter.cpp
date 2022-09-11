#include <stdafx.h>

#include "ProcessFilter.h"

NTSTATUS ProcessFilter::Init(_In_ PDRIVER_OBJECT DriverObject, _In_ PUNICODE_STRING RegistryPath)
{
	DbgPrint("%s(%p, %p)\n", __FUNCTION__, DriverObject, RegistryPath);
	return STATUS_SUCCESS;
}
NTSTATUS ProcessFilter::Start()
{
	DbgPrint("%s()\n", __FUNCTION__);
	return STATUS_SUCCESS;
}
void ProcessFilter::Stop()
{
	DbgPrint("%s()\n", __FUNCTION__);
}
void ProcessFilter::Uninit()
{
	DbgPrint("%s()\n", __FUNCTION__);
}
