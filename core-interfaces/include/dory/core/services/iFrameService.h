#pragma once

#include <dory/generic/baseTypes.h>
#include <dory/core/resources/dataContext.h>
#include <dory/core/services/iPipelineService.h>

namespace dory::core::services
{
    class IFrameService: public generic::Interface
    {
    public:
        virtual void startLoop(resources::DataContext& context, IPipelineService* pipelineService) = 0;
        virtual void endLoop() = 0;
    };
}