#pragma once

#include "dory/engine/resources/configuration.h"
#include "logService.h"
#include "fileService.h"
#include "serializationService.h"

namespace dory::engine::services::localization
{
    template<typename TImplementation>
    class ILocalizationService: NonCopyable, public StaticInterface<TImplementation>
    {
    public:
        template<typename TLocalization>
        void load(const resources::configuration::Configuration& configuration, TLocalization& localization)
        {
            this->toImplementation()->loadImpl(configuration, localization);
        }
    };

    template<typename TLogger, typename TFileService, typename TSerializationServiceBundle, typename TFormatConverter>
    class LocalizationService: public ILocalizationService<LocalizationService<TLogger, TFileService,
            TSerializationServiceBundle, TFormatConverter>>
    {
    private:
        using LoggerType = ILogService<TLogger>;
        LoggerType& logger;

        using FileServiceType = IFileService<TFileService>;
        FileServiceType& fileService;

        using SerializationServiceBundleType = serialization::ISerializationServiceBundle<typename TFormatConverter::FormatType, TSerializationServiceBundle>;
        SerializationServiceBundleType& serializationServiceBundle;

        using FormatConverterType = serialization::IFormatKeyConverter<typename TFormatConverter::FormatType, TFormatConverter>;
        FormatConverterType& formatKeyConverter;

    public:
        explicit LocalizationService(LoggerType& logger,
                                     FileServiceType& fileService,
                                     SerializationServiceBundleType& serializationServiceBundle,
                                     FormatConverterType& formatKeyConverter):
            logger(logger),
            fileService(fileService),
            serializationServiceBundle(serializationServiceBundle),
            formatKeyConverter(formatKeyConverter)
        {}

        template<typename TLocalization>
        void loadImpl(const resources::configuration::Configuration& configuration, TLocalization& localization)
        {
            auto& activeLanguage = configuration.userInterface.activeLanguage;
            if(configuration.localizations.contains(activeLanguage))
            {
                auto& localizationFiles = configuration.localizations.at(activeLanguage);
                for(std::filesystem::path fileName : localizationFiles)
                {
                    try
                    {
                        logger.information(fmt::format("load localization from: {0}", fileName.string()));

                        auto source = fileService.read(fileName);
                        serializationServiceBundle.deserialize(formatKeyConverter.getFormat(fileName), source, localization);
                    }
                    catch (const std::exception& e)
                    {
                        logger.error(fmt::format("cannot load localization: {0}", e.what()));
                    }
                    catch(...)
                    {
                        logger.error("cannot load localization: unknown exception type");
                    }
                }
            }
        }
    };
}