#pragma once

#include "base/dependencies.h"
#include "base/typeComponents.h"
#include "base/serialization/objectVisitor.h"
#include "base/domain/configuration.h"
#include "logService.h"
#include "fileService.h"
#include "serializationService.h"

namespace dory::domain::services::configuration
{
    template<typename TImplementation>
    class IConfigurationService: Uncopyable, public StaticInterface<TImplementation>
    {
    public:
        template<typename TConfiguration>
        bool load(const std::filesystem::path& configurationPath, TConfiguration& configuration)
        {
            return this->toImplementation()->loadImpl(configurationPath, configuration);
        }

        template<typename TConfiguration>
        bool save(const std::filesystem::path& configurationPath, const TConfiguration& configuration)
        {
            return this->toImplementation()->saveImpl(configurationPath, configuration);
        }

        template<typename TConfiguration>
        void save(const TConfiguration& configuration)
        {
            this->toImplementation()->saveImpl(configuration);
        }
    };

    template<typename TLoader>
    struct ConfigurationSectionContext
    {
        using LoaderType = IConfigurationService<TLoader>;
        LoaderType& loader;

        explicit ConfigurationSectionContext(LoaderType& loader):
                loader(loader)
        {}
    };

    template<typename T>
    concept IsRecursiveSectionMetadata = requires (T x)
    {
        requires std::is_base_of_v<dory::configuration::RecursiveSection, T>;
    };

    template<typename T>
    concept IsRecursiveSection = requires (T x)
    {
        { x.section };
        requires IsRecursiveSectionMetadata<std::decay_t<decltype(x.section)>>;
    };

    struct LoadConfigurationSectionObjectPolicy: public dory::serialization::DefaultObjectPolicy
    {
        template<typename TContext, typename T>
        requires (!IsRecursiveSection<T> && !IsRecursiveSectionMetadata<T>)
        inline static bool beginObject(T&& object, TContext& context)
        {
            //skip non-recursive section
            return true;
        }

        template<typename TContext, typename T>
        requires (IsRecursiveSectionMetadata<T>)
        inline static bool beginObject(T&& object, TContext& context)
        {
            //skip recursive section metadata itself
            return false;
        }

        template<typename T, typename TLoader>
        requires IsRecursiveSection<T>
        inline static bool beginObject(T&& object, ConfigurationSectionContext<TLoader>& context)
        {
            //recursive section
            auto overrideWithFiles = std::vector<std::string>{ std::move(object.section.loadFrom) };
            for(const auto& settingsFile : overrideWithFiles)
            {
                context.loader.load(settingsFile, std::forward<T>(object));
            }

            return true;
        }
    };

    struct SaveConfigurationSectionObjectPolicy: public dory::serialization::DefaultObjectPolicy
    {
        template<typename TContext, typename T>
        requires (!IsRecursiveSection<T> && !IsRecursiveSectionMetadata<T>)
        inline static bool beginObject(T&& object, TContext& context)
        {
            //skip non-recursive section
            return true;
        }

        template<typename TContext, typename T>
        requires (IsRecursiveSectionMetadata<T>)
        inline static bool beginObject(T&& object, TContext& context)
        {
            //skip recursive section metadata itself
            return false;
        }

        template<typename T, typename TLoader>
        requires IsRecursiveSection<T>
        inline static bool beginObject(T&& object, ConfigurationSectionContext<TLoader>& context)
        {
            //recursive section
            auto& saveTo = object.section.saveTo;
            if(!saveTo.empty())
            {
                context.loader.save(saveTo, object);
            }

            return true;
        }
    };

    struct LoadConfigurationSectionPolicies: public dory::serialization::VisitorDefaultPolicies
    {
        using ObjectPolicy = LoadConfigurationSectionObjectPolicy;
    };

    struct SaveConfigurationSectionPolicies: public dory::serialization::VisitorDefaultPolicies
    {
        using ObjectPolicy = SaveConfigurationSectionObjectPolicy;
    };

    template<typename TLogger, typename TFileService, typename TSerializationServiceBundle, typename TFormat, typename TFormatConverter>
    class ConfigurationService: public IConfigurationService<ConfigurationService<TLogger, TFileService, TSerializationServiceBundle, TFormat, TFormatConverter>>
    {
    private:
        using LoggerType = ILogService<TLogger>;
        LoggerType& logger;

        using FileServiceType = IFileService<TFileService>;
        FileServiceType& fileService;

        using SerializationServiceBundleType = serialization::ISerializationServiceBundle<TSerializationServiceBundle>;
        SerializationServiceBundleType& serializationServiceBundle;

        using FormatConverterType = serialization::IFormatKeyConverter<TFormat, TFormatConverter>;
        FormatConverterType& formatKeyConverter;

    public:
        explicit ConfigurationService(LoggerType& logger,
                                      FileServiceType& fileService,
                                      SerializationServiceBundleType& serializationServiceBundle,
                                      FormatConverterType& formatConverter):
                logger(logger),
                fileService(fileService),
                serializationServiceBundle(serializationServiceBundle),
                formatKeyConverter(formatConverter)
        {}

        template<typename TConfiguration>
        bool loadImpl(const std::filesystem::path& configurationPath, TConfiguration& configuration)
        {
            try
            {
                logger.information(fmt::format("load configuration from: {0}", configurationPath.string()));

                auto source = fileService.read(configurationPath);
                serializationServiceBundle.deserialize(formatKeyConverter.getFormat(configurationPath), source, configuration);

                //load recursive sections
                auto context = ConfigurationSectionContext<ConfigurationService>{ *this };
                dory::serialization::ObjectVisitor<LoadConfigurationSectionPolicies>::visit(configuration, context);

                return true;
            }
            catch(const std::exception& e)
            {
                logger.error(fmt::format("cannot load configuration: {0}", e.what()));
            }
            catch(...)
            {
                logger.error("cannot load configuration: unknown exception type");
            }

            return false;
        }

        template<typename TConfiguration>
        void saveImpl(TConfiguration& configuration)
        {
            auto context = ConfigurationSectionContext<ConfigurationService>{*this };
            dory::serialization::ObjectVisitor<SaveConfigurationSectionPolicies>::visit(configuration, context);
        }

        template<typename TConfiguration>
        bool saveImpl(const std::filesystem::path& configurationPath, const TConfiguration& configuration)
        {
            try
            {
                logger.information(fmt::format("save configuration to: {0}", configurationPath.string()));

                std::string content = serializationServiceBundle.serialize(formatKeyConverter.getFormat(configurationPath), configuration);
                fileService.write(configurationPath, content);

                return true;
            }
            catch(const std::exception& e)
            {
                logger.error(fmt::format("cannot save configuration: {0}", e.what()));
            }
            catch(...)
            {
                logger.error("cannot save configuration: unknown exception type");
            }

            return false;
        }
    };
}