#pragma once

namespace testApp
{
    using EntityId = dory::domain::entity::IdType;
    using namespace dory;
    using namespace domain;

    class Project: public dory::Project
    {
        private:
            std::shared_ptr<configuration::IConfiguration> configuration;
            std::shared_ptr<Engine> engine;

            std::shared_ptr<IIdFactory<EntityId>> windowIdFactory;
            std::shared_ptr<EntityRepository<openGL::GlfwWindow>> windowRespository;

            std::shared_ptr<IIdFactory<EntityId>> cameraIdFactory;
            std::shared_ptr<EntityRepository<entity::Camera>> cameraRepository;

            std::shared_ptr<IIdFactory<EntityId>> viewIdFactory;
            std::shared_ptr<EntityRepository<entity::View>> viewRepository;
            
            std::shared_ptr<IIdFactory<EntityId>> pipelineNodeIDFactory;
            std::shared_ptr<EntityRepository<entity::PipelineNode>> pipelineNodeRepository;

            std::shared_ptr<RepositoryReader<openGL::GlfwWindow>> windowRepositoryReader;
            std::shared_ptr<RepositoryReader<entity::Camera>> cameraRepositoryReader;
            std::shared_ptr<RepositoryReader<entity::View>> viewRepositoryReader;
            std::shared_ptr<RepositoryReader<entity::PipelineNode>> pipelineNodeRepositoryReader;

            std::shared_ptr<services::PipelineService> pipelineService;

            std::shared_ptr<events::EngineEventHubDispatcher> engineEventHub;
            std::shared_ptr<events::SystemConsoleEventHubDispatcher> consoleEventHub;

        protected:
            void configureProject() override;
            void configurePipeline() override;
    };
}