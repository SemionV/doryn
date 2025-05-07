#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <spdlog/fmt/fmt.h>

namespace dory::lifetime
{
    struct Object
    {
        std::size_t increment();
        std::size_t decrement();
    };

    struct AnnouncementSlot
    {};

    using ObjectReference = Object*;
    static constexpr std::size_t ThreadsCount = 4;

    AnnouncementSlot announcements[ThreadsCount];

    std::size_t getThreadId();
    void deleteObject(ObjectReference reference);
    void retire(ObjectReference reference);
    ObjectReference acquire(ObjectReference* referenceHandle, AnnouncementSlot* announcement);
    void release(AnnouncementSlot* announcement);
    ObjectReference fetchAndStore(ObjectReference* referenceHandle, ObjectReference reference);
    bool compareAndSwapInternal(ObjectReference* referenceHandle, ObjectReference expectedReference, ObjectReference desiredReference);
    std::optional<ObjectReference> eject();

    void increment(ObjectReference reference)
    {
        reference->increment();
    }

    void decrement(ObjectReference reference)
    {
        if(reference->decrement() == 1)
        {
            deleteObject(reference);
        }
    }

    void retireAndEject(ObjectReference reference)
    {
        retire(reference);
        if(std::optional<ObjectReference> ejectedReference = eject())
        {
            decrement(*ejectedReference);
        }
    }

    ObjectReference load(ObjectReference* referenceHandle)
    {
        AnnouncementSlot* announcement = &announcements[getThreadId()];

        ObjectReference objectReference = acquire(referenceHandle, announcement);
        if(objectReference != nullptr)
        {
            increment(objectReference);
        }

        release(announcement);

        return objectReference;
    }

    void store(ObjectReference* referenceHandle, ObjectReference desiredReference)
    {
        if(desiredReference != nullptr)
        {
            increment(desiredReference);
        }

        ObjectReference currentReference = fetchAndStore(referenceHandle, desiredReference);
        if(currentReference != nullptr)
        {
            retireAndEject(currentReference);
        }
    }

    bool compareAndSwap(ObjectReference* referenceHandle, ObjectReference expectedReference, ObjectReference desiredReference)
    {
        bool result = false;
        AnnouncementSlot* announcement = &announcements[getThreadId()];

        ObjectReference reference = acquire(&desiredReference, announcement);

        if(compareAndSwapInternal(referenceHandle, expectedReference, desiredReference))
        {
            if(desiredReference != nullptr)
            {
                increment(desiredReference);
            }

            if(expectedReference != nullptr)
            {
                retireAndEject(expectedReference);
            }

            result = true;
        }

        release(announcement);
        return result;
    }

    void destruct(ObjectReference reference)
    {
        if(reference != nullptr)
        {
            decrement(reference);
        }
    }
}