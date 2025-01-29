#pragma once

#include <filesystem>
#include <dory/generic/baseTypes.h>
#include <dory/core/resources/scene/configuration.h>

namespace dory::core::services
{
    class ISceneConfigurationService: public generic::Interface
    {
    public:
        virtual void load(const std::filesystem::path& filename, resources::scene::configuration::Scene& configuration) = 0;
        virtual void save(const std::filesystem::path& filename, const resources::scene::configuration::Scene& configuration) = 0;
    };
}