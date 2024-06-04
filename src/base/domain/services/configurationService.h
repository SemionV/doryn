#pragma once

#include "base/dependencies.h"
#include "base/typeComponents.h"
#include "base/domain/configuration.h"
#include "base/serialization/yamlDeserializer.h"
#include "base/io.h"
#include "logService.h"
#include "base/serialization/objectVisitor.h"

namespace dory::domain::services::configuration
{
    template<typename TImplementation>
    class IConfigurationLoader: Uncopyable, public StaticInterface<TImplementation>
    {
    public:
        template<typename TConfiguration>
        bool load(const std::filesystem::path& configurationPath, TConfiguration& configuration)
        {
            return this->toImplementation()->loadImpl(configurationPath, configuration);
        }
    };

    template<typename TLoader>
    struct ConfigurationSectionContext
    {
        using LoaderType = IConfigurationLoader<TLoader>;
        LoaderType& loader;

        explicit ConfigurationSectionContext(LoaderType& loader):
                loader(loader)
        {}
    };

    struct ConfigurationSectionObjectPolicy
    {
        template<typename TContext, typename T>
        requires((!std::is_base_of_v<dory::configuration::RecursiveSection, std::decay_t<T>>))
        inline static bool beginObject(T&& object, TContext& context)
        {
            //skip non-recursive section
            return true;
        }

        template<typename T, typename TLoader>
        requires(std::is_base_of_v<dory::configuration::RecursiveSection, std::decay_t<T>>)
        inline static bool beginObject(T&& object, ConfigurationSectionContext<TLoader>& context)
        {
            //recursive section
            auto overrideWithFiles = std::vector<std::string>{ std::move(object.loadFrom) };
            for(const auto& settingsFile : overrideWithFiles)
            {
                context.loader.load(settingsFile, std::forward<T>(object));
            }

            return true;
        }

        template<typename TContext, typename T>
        inline static void endObject(T&& object, TContext& context)
        {
        }
    };

    struct ConfigurationSectionLoadPolicies: public serialization::VisitorDefaultPolicies
    {
        using ObjectPolicy = ConfigurationSectionObjectPolicy;
    };

    template<typename TLogger>
    class YamlConfigurationLoader: public IConfigurationLoader<YamlConfigurationLoader<TLogger>>
    {
    private:
        using LoggerType = ILogService<TLogger>;
        LoggerType& logger;

        template<typename TConfiguration>
        void loadSections(TConfiguration& configuration)
        {
            auto context = ConfigurationSectionContext<YamlConfigurationLoader<TLogger>>{ *this };
            serialization::ObjectVisitor<ConfigurationSectionLoadPolicies>::visit(configuration, context);
        }

    public:
        explicit YamlConfigurationLoader(LoggerType& logger):
            logger(logger)
        {}

        template<typename TConfiguration>
        bool loadImpl(const std::filesystem::path& configurationPath, TConfiguration& configuration)
        {
            try
            {
                logger.information(fmt::format("load configuration from: {0}", configurationPath.string()));

                try
                {
                    auto yamlSource = getTextFileContent(configurationPath);
                    dory::serialization::yaml::deserialize(yamlSource, configuration);
                }
                catch(std::exception e)
                {
                    logger.error("cannot load configuration: " + configurationPath.string());
                }

                //load recursive settings
                loadSections(configuration);

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
    };
}