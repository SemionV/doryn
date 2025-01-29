#include <dory/core/registry.h>
#include <dory/core/services/sceneConfigurationService.h>
#include <spdlog/fmt/fmt.h>

namespace dory::core::services
{
    using namespace resources;
    using namespace resources::scene;

    SceneConfigurationService::SceneConfigurationService(Registry& registry): DependencyResolver(registry)
    {}

    void SceneConfigurationService::load(const std::filesystem::path& filename, scene::configuration::Scene& configuration)
    {
        auto fileService = _registry.get<IFileService>();
        auto dataFormatResolver = _registry.get<IDataFormatResolver>();

        if(fileService && dataFormatResolver)
        {
            try
            {
                const std::string data = fileService->read(filename);
                DataFormat dataFormat = dataFormatResolver->resolveFormat(filename);

                if(auto serializer = _registry.get<serialization::ISerializer>(dataFormat))
                {
                    serializer->deserialize(data, configuration);
                }
            }
            catch(const std::exception& e)
            {
                _registry.get<ILogService>([&e, &filename](ILogService* logger)
                {
                    logger->warning(fmt::format("Cannot load scene configuration({}): {}", filename.c_str(), e.what()));
                });
            }
        }
    }

    void SceneConfigurationService::save(const std::filesystem::path& filename, const scene::configuration::Scene& configuration)
    {
        auto fileService = _registry.get<IFileService>();
        auto dataFormatResolver = _registry.get<IDataFormatResolver>();

        if(fileService && dataFormatResolver)
        {
            try
            {
                DataFormat dataFormat = dataFormatResolver->resolveFormat(filename);

                if(auto serializer = _registry.get<serialization::ISerializer>(dataFormat))
                {
                    const auto data = serializer->serialize(configuration);
                    fileService->write(filename, data);
                }
            }
            catch(const std::exception& e)
            {
                _registry.get<ILogService>([&e, &filename](ILogService* logger)
                {
                    logger->warning(fmt::format("Cannot save scene configuration({}): {}", filename.c_str(), e.what()));
                });
            }
        }
    }
}
