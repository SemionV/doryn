#pragma once

#include "base/domain/object.h"
#include "base/entityRepository.h"

namespace dory::domain::services
{
    class PipelineService
    {
        private:
            std::shared_ptr<RepositoryReader<entity::PipelineGroup>> groupsReader;
            std::shared_ptr<RepositoryReader<entity::PipelineNode>> nodeReader;

        public:
            PipelineService(std::shared_ptr<RepositoryReader<entity::PipelineGroup>> groupsReader,
                std::shared_ptr<RepositoryReader<entity::PipelineNode>> nodeReader):
                groupsReader(groupsReader),
                nodeReader(nodeReader)
            {}

            virtual std::unique_ptr<object::Pipeline> getPipeline();
    };
}