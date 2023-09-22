#pragma once

#include "base/domain/object.h"
#include "base/domain/entityRepository.h"

namespace dory::domain::services
{
    class PipelineService
    {
        private:
            std::shared_ptr<RepositoryReader<entity::PipelineNode>> nodeReader;

        public:
            PipelineService(std::shared_ptr<RepositoryReader<entity::PipelineNode>> nodeReader):
                nodeReader(nodeReader)
            {}

            virtual std::list<std::shared_ptr<object::PipelineNode>> getPipeline();

        private:
            std::shared_ptr<object::PipelineNode> loadNode(entity::PipelineNode& nodeEntity);
    };
}