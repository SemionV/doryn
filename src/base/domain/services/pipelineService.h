#pragma once

#include "base/domain/object.h"
#include "base/domain/entityRepository.h"
#include "base/doryExport.h"

namespace dory::domain::services
{
    class DORY_API PipelineService
    {
        private:
            std::shared_ptr<RepositoryReader<entity::PipelineNode>> nodeReader;

        public:
            PipelineService(std::shared_ptr<RepositoryReader<entity::PipelineNode>> nodeReader):
                nodeReader(nodeReader)
            {}

            std::list<std::shared_ptr<object::PipelineNode>> getPipeline();

        private:
            std::shared_ptr<object::PipelineNode> loadNode(const entity::PipelineNode& nodeEntity);
    };
}