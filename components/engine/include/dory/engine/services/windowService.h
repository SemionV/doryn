#pragma once

namespace dory::engine::services
{
    template<typename TImplementation>
    class IWindowService: NonCopyable, public StaticInterface<TImplementation>
    {
    public:
        auto createWindow()
        {
            return this->toImplementation()->createWindowImpl();
        }

        void closeWindow(resources::entity::IdType windowId)
        {
            this->toImplementation()->closeWindowImpl(windowId);
        }
    };
}