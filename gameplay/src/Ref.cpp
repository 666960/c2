/*
 * Ref.cpp
 */

#include "Base.h"
#include "Ref.h"
#include "Game.h"

namespace gameplay
{

#ifdef GAMEPLAY_MEM_LEAK_DETECTION
void* trackRef(Ref* ref);
void untrackRef(Ref* ref, void* record);
#endif

Ref::Ref() :
    _refCount(1)
{
#ifdef GAMEPLAY_MEM_LEAK_DETECTION
    __record = trackRef(this);
#endif
}

Ref::~Ref()
{
}

void Ref::addRef()
{
    ++_refCount;
}

void Ref::release()
{
    if ((--_refCount) <= 0)
    {
#ifdef GAMEPLAY_MEM_LEAK_DETECTION
        untrackRef(this, __record);
#endif
        delete this;
    }
}

unsigned int Ref::getRefCount() const
{
    return _refCount;
}

#ifdef GAMEPLAY_MEM_LEAK_DETECTION

struct RefAllocationRecord
{
    Ref* ref;
    RefAllocationRecord* next;
    RefAllocationRecord* prev;
};

RefAllocationRecord* __refAllocations = 0;
int __refAllocationCount = 0;

void Ref::printLeaks()
{
    // Dump Ref object memory leaks
    if (__refAllocationCount == 0)
    {
        printError("[memory] All Ref objects successfully cleaned up (no leaks detected).");
    }
    else
    {
        printError("[memory] WARNING: %d Ref objects still active in memory.", __refAllocationCount);
        for (RefAllocationRecord* rec = __refAllocations; rec != NULL; rec = rec->next)
        {
            Ref* ref = rec->ref;
            const char* type = typeid(*ref).name();
            printError("[memory] LEAK: Ref object '%s' still active with reference count %d.", (type ? type : ""), ref->getRefCount());
        }
    }
}

void* trackRef(Ref* ref)
{
    // Create memory allocation record
    RefAllocationRecord* rec = (RefAllocationRecord*)malloc(sizeof(RefAllocationRecord));
    rec->ref = ref;
    rec->next = __refAllocations;
    rec->prev = 0;

    if (__refAllocations)
        __refAllocations->prev = rec;
    __refAllocations = rec;
    ++__refAllocationCount;

    return rec;
}

void untrackRef(Ref* ref, void* record)
{
    RefAllocationRecord* rec = (RefAllocationRecord*)record;
    if (rec->ref != ref)
    {
        printError("[memory] CORRUPTION: Attempting to free Ref with invalid ref tracking record.");
        return;
    }

    // Link this item out
    if (__refAllocations == rec)
        __refAllocations = rec->next;
    if (rec->prev)
        rec->prev->next = rec->next;
    if (rec->next)
        rec->next->prev = rec->prev;
    free((void*)rec);
    --__refAllocationCount;
}

#endif

}
