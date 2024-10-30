#include "dory/game/bootstrap.h"

namespace dory::game
{

    bool Bootstrap::initialize(const dory::generic::extension::LibraryHandle& libraryHandle, core::resources::DataContext& context, core::Registry& registry)
    {
        registry.get<dory::core::services::IMultiSinkLogService>(dory::core::resources::Logger::Config, [&context, &registry](dory::core::services::IMultiSinkLogService* logger){
            logger->initialize(context.configuration.loggingConfiguration.configurationLogger, registry);
        });

        registry.get<dory::core::services::IConfigurationService>([&context](dory::core::services::IConfigurationService* configurationService){
            configurationService->load(context.configuration);
        });

        registry.get<dory::core::services::IMultiSinkLogService, dory::core::resources::Logger::App>([&context, &registry](dory::core::services::IMultiSinkLogService* logger){
            logger->initialize(context.configuration.loggingConfiguration.mainLogger, registry);
        });

        registry.get<dory::core::services::ILocalizationService>([&context](dory::core::services::ILocalizationService* localizationService){
            localizationService->load(context.configuration, context.localization);
        });

        registry.get<
                dory::generic::registry::Service<dory::core::services::ILogService, dory::core::resources::Logger::App>,
                dory::generic::registry::Service<dory::core::devices::IStandardIODevice>,
                dory::generic::registry::Service<dory::core::devices::ITerminalDevice>>(
        [&context](
        dory::core::services::ILogService* logger,
        dory::core::devices::IStandardIODevice* ioDevice,
        dory::core::devices::ITerminalDevice* terminalDevice)
        {
            logger->information(fmt::format("Dory Game, {0}.{1}, {2}",
                                            context.configuration.buildInfo.version,
                                            context.configuration.buildInfo.commitSHA,
                                            context.configuration.buildInfo.timestamp));

            logger->information(context.localization.hello);
            logger->information(context.localization.goodBye->get("Semion"));
            logger->information(context.localization.birthDate->get(11, 03, 1984));

            ioDevice->connect(context);
            terminalDevice->connect(context);
            terminalDevice->enterCommandMode();
        });

        registry.get<dory::core::repositories::IPipelineRepository>([&registry, &libraryHandle](dory::core::repositories::IPipelineRepository* pipelineRepository){
            auto submitInputEvents = [&registry](auto referenceId, const auto& timeStep, dory::core::resources::DataContext& context){
                registry.get<dory::core::events::io::Bundle::IDispatcher>([&context](dory::core::events::io::Bundle::IDispatcher* dispatcher){
                    dispatcher->fireAll(context);
                });
            };
            auto resourceHandle = dory::generic::extension::ResourceHandle<dory::core::resources::entity::PipelineNode::UpdateFunctionType>{ libraryHandle, submitInputEvents };
            auto node = dory::core::resources::entity::PipelineNode(resourceHandle);
            pipelineRepository->addNode(node);

            auto flushOutput = [&registry](auto referenceId, const auto& timeStep, dory::core::resources::DataContext& context){
                registry.get<dory::core::devices::IStandardIODevice>([](dory::core::devices::IStandardIODevice* ioDevice){
                    ioDevice->flush();
                });
            };
            resourceHandle = dory::generic::extension::ResourceHandle<dory::core::resources::entity::PipelineNode::UpdateFunctionType>{ libraryHandle, flushOutput };
            node = dory::core::resources::entity::PipelineNode(resourceHandle);
            pipelineRepository->addNode(node);
        });

        registry.get<dory::core::services::IScriptService>([](dory::core::services::IScriptService* scriptService){

        });

        return true;
    }

    bool Bootstrap::run(core::resources::DataContext& context, core::Registry& registry)
    {
        registry.get<dory::core::services::IFrameService>([&context](dory::core::services::IFrameService* frameService) {
            frameService->startLoop(context);
        });

        return true;
    }
}