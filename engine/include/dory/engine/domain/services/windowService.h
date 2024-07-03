#pragma once

#include "base/typeComponents.h"

namespace dory::domain::services
{
    template<typename TImplementation>
    class IWindowService: NonCopyable, public StaticInterface<TImplementation>
    {
    public:
        auto createWindow()
        {
            return this->toImplementation()->createWindowImpl();
        }

        void closeWindow(entity::IdType windowId)
        {
            this->toImplementation()->closeWindowImpl(windowId);
        }
    };
}