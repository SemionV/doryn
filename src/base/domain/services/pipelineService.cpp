#include "pipelineService.h"

namespace dory::domain::services
{
    std::unique_ptr<object::Pipeline> PipelineService::getPipeline()
    {
        auto pipelineObject = std::make_unique<object::Pipeline>();

        auto groupEntitiesIterator = groupsReader->getTraverseIterator();
        
        auto groupEntity = groupEntitiesIterator->next();
        while(groupEntity)
        {
            auto pipelineGroupObject = std::make_shared<object::PipelineGroup>();
            pipelineObject->groups.emplace_back(pipelineGroupObject);
            pipelineGroupObject->groupEntity = *groupEntity;

            groupEntity = groupEntitiesIterator->next();
        }

        return pipelineObject;
    }
}