#pragma once

#include <filesystem>
#include <dory/generic/typeList.h>
#include <dory/core/resources/configuration.h>

namespace dory::core::services
{
    template<typename T>
    class IConfigurationServiceGeneric
    {
    public:
        virtual bool load(const std::filesystem::path& configurationPath, T& configuration, resources::DataContext& dataContext) = 0;
        virtual void load(T& configuration, resources::DataContext& dataContext) = 0;
        virtual bool save(const std::filesystem::path& configurationPath, const T& configuration, resources::DataContext& dataContext) = 0;
        virtual void save(const T& configuration, resources::DataContext& dataContext) = 0;
    };

    template<typename... Ts>
    class IConfigurationServiceBundle: public IConfigurationServiceGeneric<Ts>...
    {
    public:
        using Types =  generic::TypeList<Ts...>;

        using IConfigurationServiceGeneric<Ts>::load...;
        using IConfigurationServiceGeneric<Ts>::save...;

        virtual ~IConfigurationServiceBundle() = default;
    };

    template<typename... Ts>
    class IConfigurationServiceBundle<generic::TypeList<Ts...>>: public IConfigurationServiceBundle<Ts...>
    {};

    using ConfigurationTypes = generic::TypeList<
            resources::configuration::Configuration,
            resources::configuration::Interface>;

    using IConfigurationService = IConfigurationServiceBundle<ConfigurationTypes>;
}
