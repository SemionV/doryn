#pragma once

#include <dory/core/services/iSerializer.h>
#include <dory/core/implementation.h>
#include <dory/serialization/yamlSerializer.h>
#include <dory/serialization/yamlDeserializer.h>
#include <dory/serialization/jsonSerializer.h>
#include <dory/serialization/jsonDeserializer.h>

namespace dory::core::services::serialization
{
    template<typename T, typename TPolicy, typename TState>
    class YamlSerializerGeneric: public implementation::ImplementationLevel<TPolicy, TState>
    {
    public:
        inline std::string serialize(const T& object) final
        {
            return dory::serialization::yaml::serialize(object);
        }

        inline void deserialize(const std::string& source, T& object) final
        {
            dory::serialization::yaml::deserialize(source, object);
        }
    };

    struct YamlSerializerPolicy: implementation::ImplementationPolicy<implementation::ImplementationList<YamlSerializerGeneric>>
    {};

    using YamlSerializer = implementation::Implementation<generic::TypeList<ISerializer>, ISerializer::Types, YamlSerializerPolicy>;

    template<typename T, typename TPolicy, typename TState>
    class JsonSerializerGeneric: public implementation::ImplementationLevel<TPolicy, TState>
    {
    public:
        inline std::string serialize(const T& object) final
        {
            return dory::serialization::json::serialize(object);
        }

        inline void deserialize(const std::string& source, T& object) final
        {
            dory::serialization::json::deserialize(source, object);
        }
    };

    struct JsonSerializerPolicy: implementation::ImplementationPolicy<implementation::ImplementationList<JsonSerializerGeneric>>
    {};

    using JsonSerializer = implementation::Implementation<generic::TypeList<ISerializer>, ISerializer::Types, JsonSerializerPolicy>;
}
