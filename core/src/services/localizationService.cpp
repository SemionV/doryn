#include <dory/core/services/localizationService.h>
#include <filesystem>
#include <spdlog/fmt/fmt.h>
#include <dory/core/registry.h>

namespace dory::core::services
{
    LocalizationService::LocalizationService(Registry &registry):
            _registry(registry)
    {}

    void LocalizationService::load(const resources::configuration::Configuration& configuration, resources::Localization& localization, resources::DataContext& dataContext)
    {
        auto& activeLanguage = configuration.userInterface.activeLanguage;
        if(configuration.localizations.contains(activeLanguage))
        {
            auto& localizationFiles = configuration.localizations.at(activeLanguage);
            for(std::filesystem::path fileName : localizationFiles)
            {
                try
                {
                    _registry.get<ILogService, resources::Logger::App>([&fileName](ILogService* logger)
                    {
                        logger->information(fmt::format("load localization from: {0}", fileName.string()));
                    });

                    std::string data {};
                    _registry.get<IFileService>([&fileName, &data](IFileService* fileService)
                    {
                        data = fileService->read(fileName);
                    });

                    resources::DataFormat dataFormat {};
                    _registry.get<IDataFormatResolver>([&fileName, &dataFormat](IDataFormatResolver* resolver)
                    {
                        dataFormat = resolver->resolveFormat(fileName);
                    });

                    _registry.get<serialization::ISerializer>(dataFormat, [this, &dataContext, &data, &localization](serialization::ISerializer* serializer)
                    {
                        serializer->deserialize(data, localization, _registry, dataContext);
                    });
                }
                catch (const std::exception& e)
                {
                    _registry.get<ILogService, resources::Logger::App>([&e](ILogService* logger)
                    {
                        logger->error(fmt::format("cannot load localization: {0}", e.what()));
                    });
                }
                catch(...)
                {
                    _registry.get<ILogService, resources::Logger::App>([](ILogService* logger)
                    {
                        logger->error(std::string_view{"cannot load localization: unknown exception type"});
                    });
                }
            }
        }
    }
}