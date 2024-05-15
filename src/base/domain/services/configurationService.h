#pragma once

#include "base/dependencies.h"
#include "base/typeComponents.h"
#include "base/domain/configuration.h"
#include "base/serialization/yamlDeserializer.h"
#include "base/io.h"

namespace dory::domain::services
{
    template<typename TImplementation>
    class IConfigurationLoader: Uncopyable, public StaticInterface<TImplementation>
    {
    public:
        void load(const std::filesystem::path& configurationPath, TImplementation::ConfigurationType& configuration)
        {
            this->toImplementation()->loadImpl(configuration);
        }
    };

    template<typename TConfiguration>
    class YamlConfigurationLoader: public IConfigurationLoader<YamlConfigurationLoader<TConfiguration>>
    {
    public:
        using ConfigurationType = TConfiguration;

        void loadImpl(const std::filesystem::path& configurationPath, ConfigurationType& configuration)
        {
            auto yamlSource = getTextFileContent(configurationPath);
            dory::typeMap::yaml::YamlDeserializer::deserialize(yamlSource, configuration);
        }
    };
}