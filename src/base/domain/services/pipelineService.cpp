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
            pipelineGroupObject->groupEntity = groupEntity;

            std::list<entity::PipelineNode*> nodeEntities;
            nodeReader->list(groupEntity->id, [](entity::PipelineNode* nodeEntity, entity::IdType groupId)
            {
                return nodeEntity->groupId == groupId;
            }, pipelineGroupObject->nodeEntities);
            
            pipelineGroupObject->nodeEntities.sort([](entity::PipelineNode* a, entity::PipelineNode* b)
            {
                return a->priority < b->priority;
            });

            groupEntity = groupEntitiesIterator->next();
            nodeEntities.clear();
        }

        pipelineObject->groups.sort([](std::shared_ptr<object::PipelineGroup> a, std::shared_ptr<object::PipelineGroup> b)
        {
            return a->groupEntity->priority < b->groupEntity->priority;
        });


        return pipelineObject;
    }
}