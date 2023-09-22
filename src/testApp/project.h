#pragma once

#include "projectDataContext.h"

namespace testApp
{
    using EntityId = dory::domain::entity::IdType;
    using namespace dory;
    using namespace domain;

    class Project: public dory::Project<ProjectDataContext>
    {
        private:
            std::shared_ptr<configuration::IConfiguration> configuration;

            std::shared_ptr<IIdFactory<EntityId>> windowIdFactory;
            std::shared_ptr<EntityRepository<openGL::GlfwWindow>> windowRespository;
            std::shared_ptr<RepositoryReader<openGL::GlfwWindow>> windowRepositoryReader;

            std::shared_ptr<IIdFactory<EntityId>> cameraIdFactory;
            std::shared_ptr<EntityRepository<entity::Camera>> cameraRepository;
            std::shared_ptr<RepositoryReader<entity::Camera>> cameraRepositoryReader;

            std::shared_ptr<IIdFactory<EntityId>> viewIdFactory;
            std::shared_ptr<EntityRepository<entity::View>> viewRepository;
            std::shared_ptr<RepositoryReader<entity::View>> viewRepositoryReader;
            
            std::shared_ptr<IIdFactory<EntityId>> pipelineNodeIdFactory;
            std::shared_ptr<EntityRepository<entity::PipelineNode>> pipelineNodeRepository;
            std::shared_ptr<RepositoryReader<entity::PipelineNode>> pipelineNodeRepositoryReader;

            std::shared_ptr<services::PipelineService> pipelineService;

            std::shared_ptr<events::EngineEventHubDispatcher> engineEventHub;
            std::shared_ptr<events::SystemConsoleEventHubDispatcher> consoleEventHub;
            std::shared_ptr<events::WindowEventHubDispatcher> glfwWindowEventHub;

        protected:
            void configureProject() override;
            void attachEventHandlers() override;
            void configurePipeline(ProjectDataContext& context) override;

            std::shared_ptr<Engine> getEngine() override;
            std::shared_ptr<services::IFrameService> getFrameService() override;
            std::shared_ptr<ProjectDataContext> getDataContext() override;

        private:
            void onInitializeEngine(DataContext& context, const events::InitializeEngineEventData& eventData);
            void onConsoleKeyPressed(DataContext& context, events::KeyPressedEventData& eventData);
            void onCloseWindow(DataContext& context, events::CloseWindowEventData& eventData);
    };
}