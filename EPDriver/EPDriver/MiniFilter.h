#pragma once
#include <stdafx.h>

NTSTATUS MiniFilterRegister(PDRIVER_OBJECT DriverObject);
void MiniFilterUnregister();
