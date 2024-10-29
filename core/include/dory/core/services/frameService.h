#pragma once

#include <dory/core/services/iFrameService.h>

namespace dory::core::services
{
    class FrameService: public IFrameService
    {
    private:
        bool isStop = false;

    public:
        void startLoop(resources::DataContext& context, IPipelineService* pipelineService) override;
        void endLoop() override;
    };
}
