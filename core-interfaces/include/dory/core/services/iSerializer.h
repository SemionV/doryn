#pragma once

#include <string>
#include <dory/generic/typeList.h>
#include <dory/core/resources/dataFormat.h>
#include <dory/core/resources/configuration.h>

namespace dory::core::services::serialization
{
    template<typename T>
    class ISerializerGeneric
    {
    public:
        virtual std::string serialize(const T& object) = 0;
        virtual void deserialize(const std::string& source, T& object) = 0;
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
            resources::configuration::Interface>;

    using ISerializer = ISerializerBundle<SerializationTypes>;
}
