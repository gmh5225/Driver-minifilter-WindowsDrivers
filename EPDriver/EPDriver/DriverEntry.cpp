#include "stdafx.h"
#include "MiniFilter.h"

#include "Filters/ProcessFilter.h"
#include "Plugins/EventCollector/EventCollector.h"

extern "C" __declspec(dllexport) DRIVER_INITIALIZE DriverEntry;
DRIVER_UNLOAD DriverUnload;

template<typename... T>
class DelegateOf
{
public:
	static NTSTATUS Init(_Inout_ PDRIVER_OBJECT DriverObject, _In_ PUNICODE_STRING RegistryPath)
	{
		// Referenced: https://stackoverflow.com/a/25683817
		NTSTATUS rets[] = { STATUS_SUCCESS, T::Init(DriverObject, RegistryPath)... };
		for (auto ret : rets)
			if (ret != STATUS_SUCCESS)
			{
				int release[] = { 0, (T::Uninit(), 0)...};
				return STATUS_UNSUCCESSFUL;
			}
		return STATUS_SUCCESS;
	}
	static NTSTATUS Start()
	{
		NTSTATUS rets[] = { T::Start()... };
		for (auto ret : rets)
			if (ret != STATUS_SUCCESS)
			{
				int release[] = { (T::Stop(), 0)... };
				return ret;
			}
		return STATUS_SUCCESS;
	}
	static void Stop() { int release[] = { (T::Stop(), 0)... }; }
	static void Uninit() { int release[] = { (T::Uninit(), 0)... }; }
};

typedef DelegateOf<
	EventCollector,
	ProcessFilter
> Delegate;

NTSTATUS DriverEntry(
	_In_ PDRIVER_OBJECT DriverObject,
	_In_ PUNICODE_STRING RegistryPath)
{
	DbgPrint("%s(%p, %ws)\n", __FUNCTION__, DriverObject, RegistryPath->Buffer);

    NTSTATUS ret = STATUS_SUCCESS;
    bool minifilter = false;
    bool features_init = false;
    
    // Setup Driver
	DriverObject->DriverUnload = DriverUnload;
	/*for (int i = 0; i < IRP_MJ_MAXIMUM_FUNCTION; i++)
		DriverObject->MajorFunction[i] = DriverDispatch;*/
    
    // Setup MiniFilter
    {
        //ret = MiniFilter::Init(DriverObject);
        //if (ret != STATUS_SUCCESS)
        //    goto CLEAN_UP;
        //minifilter = true;
    }

    // Init Features
    {
        ret = Delegate::Init(DriverObject, RegistryPath);
        if (ret == STATUS_SUCCESS)
            goto CLEAN_UP;
        features_init = true;
    }

    // Start Features
    {
        ret = Delegate::Start();
        if (ret != STATUS_SUCCESS)
            goto CLEAN_UP;
    }
   
	return STATUS_SUCCESS;

CLEAN_UP:
    if (minifilter)
        MiniFilter::Uninit();
    if (features_init)
        Delegate::Uninit();
    return ret;
}

void DriverUnload(
	_In_ PDRIVER_OBJECT DriverObject)
{
	UNREFERENCED_PARAMETER(DriverObject);

    // Stop Features
	Delegate::Stop();

    // Uninit Features
	Delegate::Uninit();

    // Remove MiniFilter
    //MiniFilter::Uninit();

	DbgPrint("%s() -> Done\n", __FUNCTION__);
}
