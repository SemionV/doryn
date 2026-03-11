#pragma once

#include <dory/types.h>
#include "janitor.h"

namespace dory::data_structures::memory_reclamation
{
    struct RetiredNode
    {
        void* ptr = nullptr;
        Janitor* janitor = nullptr;
    };

    template <typename TRetiredNode, u32 MaxRetired>
    struct RetireList
    {
        using RetiredNodeType = TRetiredNode;

        std::array<TRetiredNode, MaxRetired> nodes{};
        u32 count = 0;

        [[nodiscard]] bool full() const
        {
            return count == MaxRetired;
        }

        void push(const TRetiredNode& retiredNode)
        {
            assert::debug(count < MaxRetired, "RetireList::push: retire list is full");
            nodes[count++] = retiredNode;
        }

        template <typename Fn>
        void reclaimIf(Fn&& canReclaim)
        {
            u32 write = 0;
            for (u32 read = 0; read < count; ++read)
            {
                TRetiredNode& retiredNode = nodes[read];
                if (canReclaim(retiredNode) && retiredNode.janitor != nullptr)
                {
                    retiredNode.janitor->cleanup(retiredNode.ptr);
                }
                else
                {
                    if (write != read)
                        nodes[write] = retiredNode;
                    ++write;
                }
            }
            count = write;
        }
    };
}