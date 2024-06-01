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
        bool load(const std::filesystem::path& configurationPath, TConfiguration& configuration)
        {
            return this->toImplementation()->loadImpl(configurationPath, configuration);
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

        bool loadImpl(const std::filesystem::path& configurationPath, ConfigurationType& configuration)
        {
            try
            {
                logger.information(fmt::format("load configuration from: {0}", configurationPath.string()));

                auto yamlSource = getTextFileContent(configurationPath);
                dory::serialization::yaml::deserialize(yamlSource, configuration);

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