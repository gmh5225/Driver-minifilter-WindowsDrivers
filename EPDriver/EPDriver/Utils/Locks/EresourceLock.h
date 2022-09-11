#pragma once
#include "stdafx.h"

class EresourceLock {
private:
    ERESOURCE resource;
public:
    EresourceLock();
    ~EresourceLock();

    void AcquireExclusive();
    void AcquireShared();
    void Release();
};