#include "stdafx.h"
#include "EresourceLock.h"

EresourceLock::EresourceLock()
{
    ExInitializeResourceLite(&resource);
}
EresourceLock::~EresourceLock()
{
    ExDeleteResourceLite(&resource);
}

void EresourceLock::AcquireExclusive()
{
    KeEnterCriticalRegion();
    ExAcquireResourceExclusiveLite(&resource, TRUE);
}
void EresourceLock::AcquireShared()
{
    KeEnterCriticalRegion();
    ExAcquireResourceSharedLite(&resource, TRUE);
}

void EresourceLock::Release()
{
    ExReleaseResourceLite(&resource);
    KeLeaveCriticalRegion();
}