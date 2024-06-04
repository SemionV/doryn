#pragma once

#include "base/dependencies.h"
#include "base/typeComponents.h"
#include "base/domain/configuration.h"
#include "base/serialization/yamlDeserializer.h"
#include "base/serialization/yamlSerializer.h"
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

        template<typename TConfiguration>
        bool save(const std::filesystem::path& configurationPath, TConfiguration& configuration)
        {
            return this->toImplementation()->saveImpl(configurationPath, configuration);
        }

        template<typename TConfiguration>
        void save(TConfiguration& configuration)
        {
            this->toImplementation()->saveImpl(configuration);
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

    struct LoadConfigurationSectionObjectPolicy: public serialization::DefaultObjectPolicy
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
    };

    struct SaveConfigurationSectionObjectPolicy: public serialization::DefaultObjectPolicy
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
            if(!object.saveTo.empty())
            {
                context.loader.save(object.saveTo, object);
            }

            return true;
        }
    };

    struct LoadConfigurationSectionPolicies: public serialization::VisitorDefaultPolicies
    {
        using ObjectPolicy = LoadConfigurationSectionObjectPolicy;
    };

    struct SaveConfigurationSectionPolicies: public serialization::VisitorDefaultPolicies
    {
        using ObjectPolicy = SaveConfigurationSectionObjectPolicy;
    };

    template<typename TLogger>
    class YamlConfigurationLoader: public IConfigurationLoader<YamlConfigurationLoader<TLogger>>
    {
    private:
        using LoggerType = ILogService<TLogger>;
        LoggerType& logger;

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

                auto yamlSource = readFromFile(configurationPath);
                dory::serialization::yaml::deserialize(yamlSource, configuration);

                //load recursive sections
                auto context = ConfigurationSectionContext<YamlConfigurationLoader<TLogger>>{ *this };
                serialization::ObjectVisitor<LoadConfigurationSectionPolicies>::visit(configuration, context);

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
            auto context = ConfigurationSectionContext<YamlConfigurationLoader<TLogger>>{ *this };
            serialization::ObjectVisitor<SaveConfigurationSectionPolicies>::visit(configuration, context);
        }

        template<typename TConfiguration>
        bool saveImpl(const std::filesystem::path& configurationPath, TConfiguration& configuration)
        {
            try
            {
                logger.information(fmt::format("save configuration to: {0}", configurationPath.string()));

                auto yaml = dory::serialization::yaml::serialize(configuration);
                writeToFile(configurationPath, yaml);

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