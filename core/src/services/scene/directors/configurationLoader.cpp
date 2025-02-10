#include <dory/core/registry.h>
#include <dory/core/services/scene/directors/configurationLoader.h>

namespace dory::core::services::scene::directors
{
    using namespace resources;
    using namespace resources::scene;
    using namespace resources::scene::configuration;

    ConfigurationLoader::ConfigurationLoader(Registry& registry):
        DependencyResolver(registry)
    {}

    void ConfigurationLoader::initialize(Scene& scene, SceneConfiguration& sceneConfig, DataContext& context)
    {
        _registry.get<IMultiSinkLogService>(Logger::Config, [this, &context](IMultiSinkLogService* logger){
            logger->initialize(context.configuration.loggingConfiguration.configurationLogger, _registry);
        });

        _registry.get<IConfigurationService>([&context](IConfigurationService* configurationService){
            configurationService->load(context.configuration, context);
        });

        _registry.get<IMultiSinkLogService, Logger::App>([this, &context](IMultiSinkLogService* logger){
            logger->initialize(context.configuration.loggingConfiguration.mainLogger, _registry);
        });

        _registry.get<ILocalizationService>([&context](ILocalizationService* localizationService){
            localizationService->load(context.configuration, context.localization, context);
        });
    }

    void ConfigurationLoader::destroy(Scene& scene, DataContext& context)
    {
    }
}
