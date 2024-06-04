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
    template<typename TConfiguration, typename TImplementation>
    class IConfigurationLoader: Uncopyable, public StaticInterface<TImplementation>
    {
    public:
        bool load(const std::filesystem::path& configurationPath, TConfiguration& configuration)
        {
            return this->toImplementation()->loadImpl(configurationPath, configuration);
        }
    };

    template<template<typename TSection, typename TLogger> class TConfigurationLoader, typename TLogger>
    struct ConfigurationSectionContext
    {
        using LoggerType = ILogService<TLogger>;
        LoggerType& logger;

        explicit ConfigurationSectionContext(LoggerType& logger):
            logger(logger)
        {}
    };

    struct SectionLoadObjectPolicy
    {
        template<typename TContext, typename T>
        requires((!std::is_base_of_v<dory::configuration::RecursiveSection, std::decay_t<T>>))
        inline static bool beginObject(T&& object, TContext& context)
        {
            //skip non-recursive section
            return true;
        }

        template<typename T, typename TLogger, template<typename TSection, typename> class TConfigurationLoader>
        requires(std::is_base_of_v<dory::configuration::RecursiveSection, std::decay_t<T>>)
        inline static bool beginObject(T&& object, ConfigurationSectionContext<TConfigurationLoader, TLogger>& context)
        {
            //recursive section
            auto loader = TConfigurationLoader<std::decay_t<T>, TLogger>{ context.logger };
            auto overrideWithFiles = std::vector<std::string>{ std::move(object.overrideWithFiles) };
            for(const auto& settingsFile : overrideWithFiles)
            {
                loader.load(settingsFile, std::forward<T>(object));
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
        using ObjectPolicy = SectionLoadObjectPolicy;
    };

    template<typename TConfiguration, typename TLogger>
    class YamlConfigurationLoader: public IConfigurationLoader<TConfiguration, YamlConfigurationLoader<TConfiguration, TLogger>>
    {
    private:
        using ConfigurationType = TConfiguration;

        using LoggerType = ILogService<TLogger>;
        LoggerType& logger;

        void loadSections(ConfigurationType& configuration)
        {
            auto context = ConfigurationSectionContext<YamlConfigurationLoader, TLogger>{ logger };
            serialization::ObjectVisitor<ConfigurationSectionLoadPolicies>::visit(configuration, context);
        }

    public:
        explicit YamlConfigurationLoader(LoggerType& logger):
            logger(logger)
        {}

        bool loadImpl(const std::filesystem::path& configurationPath, ConfigurationType& configuration)
        {
            try
            {
                logger.information(fmt::format("load configuration from: {0}", configurationPath.string()));

                auto yamlSource = getTextFileContent(configurationPath);
                dory::serialization::yaml::deserialize(yamlSource, configuration);

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