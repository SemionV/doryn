#pragma once

#include <dory/generics/typeTraits.h>

namespace dory::engine::controllers
{
    template<typename TImplementation>
    class IPipelineController: NonCopyable, public StaticInterface<TImplementation>
    {
    public:
        void update(DataContextType& context, const resources::TimeSpan& timeStep)
        {
            this->toImplementation()->updateImpl(context, timeStep);
        }

        void initialize(DataContextType& context)
        {
            this->toImplementation()->initializeImpl(context);
        };

        void stop(DataContextType& context)
        {
            this->toImplementation()->stopImpl(context);
        }
    };
}