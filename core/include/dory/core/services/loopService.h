#pragma once

#include <dory/core/services/iLoopService.h>
#include <dory/core/resources/profiling.h>

namespace dory::core
{
    class Registry;
}

namespace dory::core::services
{
    class LoopService: public ILoopService
    {
    private:
        bool isStop = false;
        Registry& _registry;

        /*void printProfilingInfo(const resources::profiling::Profiling& profiling) const;
        void printProfilingDetailedInfo(const resources::profiling::Profiling& profiling) const;
        void printFrameInfo(const resources::profiling::Frame& frame) const;*/

    public:
        explicit LoopService(Registry& registry);

        void startLoop(resources::DataContext& context) override;
        void endLoop() override;
    };
}
