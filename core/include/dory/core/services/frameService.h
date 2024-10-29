#pragma once

#include <dory/core/services/iFrameService.h>

namespace dory::core
{
    class Registry;
}

namespace dory::core::services
{
    class FrameService: public IFrameService
    {
    private:
        bool isStop = false;
        Registry& _registry;

    public:
        explicit FrameService(Registry& registry);

        void startLoop(resources::DataContext& context) override;
        void endLoop() override;
    };
}
