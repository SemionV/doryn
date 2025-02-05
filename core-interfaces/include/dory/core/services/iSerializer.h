#pragma once

#include <string>
#include <dory/generic/typeList.h>
#include <dory/core/resources/configuration.h>
#include <dory/core/resources/localization.h>
#include <dory/core/resources/scene/configuration.h>
#include <dory/core/resources/dataContext.h>
#include <dory/generic/serialization.h>

namespace dory::core
{
    struct Registry;
}

namespace dory::core::services::serialization
{
    using SerializationContextPoliciesType = generic::serialization::ContextPolicies<Registry, resources::DataContext, resources::DataFormat>;

    template<typename T>
    class ISerializerGeneric
    {
    public:
        virtual std::string serialize(const T& object, Registry& registry, resources::DataContext& dataContext) = 0;
        virtual void deserialize(const std::string& source, T& object, Registry& registry, resources::DataContext& dataContext) = 0;
    };

    template<typename... Ts>
    class ISerializerBundle: public ISerializerGeneric<Ts>...
    {
    public:
        using Types =  generic::TypeList<Ts...>;

        using ISerializerGeneric<Ts>::serialize...;
        using ISerializerGeneric<Ts>::deserialize...;

        virtual ~ISerializerBundle() = default;
    };

    template<typename... Ts>
    class ISerializerBundle<generic::TypeList<Ts...>>: public ISerializerBundle<Ts...>
    {};

    using SerializationTypes = generic::TypeList<
            resources::configuration::Configuration,
            resources::configuration::Interface,
            resources::Localization,
            resources::scene::configuration::Scene>;

    using ISerializer = ISerializerBundle<SerializationTypes>;
}
