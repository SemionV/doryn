#pragma once

#include "base/dependencies.h"
#include "base/typeComponents.h"
#include "base/domain/configuration.h"
#include "base/serialization/yamlDeserializer.h"
#include "base/io.h"
#include "logService.h"

namespace dory::domain::services
{
    template<typename TConfiguration, typename TImplementation>
    class IConfigurationLoader: Uncopyable, public StaticInterface<TImplementation>
    {
    public:
        void load(const std::filesystem::path& configurationPath, TConfiguration& configuration)
        {
            this->toImplementation()->loadImpl(configuration);
        }
    };

    template<typename TConfiguration, typename TLogger>
    class YamlConfigurationLoader: public IConfigurationLoader<TConfiguration, YamlConfigurationLoader<TConfiguration, TLogger>>
    {
    private:
        using LoggerType = ILogService<TLogger>;
        LoggerType& logger;

    public:
        using ConfigurationType = TConfiguration;

        explicit YamlConfigurationLoader(LoggerType& logger):
            logger(logger)
        {}

        void loadImpl(const std::filesystem::path& configurationPath, ConfigurationType& configuration)
        {
            try
            {
                auto yamlSource = getTextFileContent(configurationPath);
                dory::typeMap::yaml::YamlDeserializer::deserialize(yamlSource, configuration);
            }
            catch(const std::exception& e)
            {
                logger.error("Cannot load configuration {0}", "!");
            }
            catch(...)
            {

            }
        }
    };
}