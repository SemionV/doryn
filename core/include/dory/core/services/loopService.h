#pragma once

#include <dory/core/services/iLoopService.h>

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

    public:
        explicit LoopService(Registry& registry);

        void startLoop(resources::DataContext& context) override;
        void endLoop() override;
    };
}
