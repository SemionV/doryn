#pragma once

#include "base/typeComponents.h"

namespace dory::domain::services
{
    template<typename TImplementation>
    class IViewService: Uncopyable, public StaticInterface<TImplementation>
    {
    public:

    };
}