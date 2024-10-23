#pragma once

#include <dory/core/services/iSerializationService.h>
#include <dory/core/generic/implementation.h>
#include <dory/serialization/yamlSerializer.h>
#include <dory/serialization/yamlDeserializer.h>
#include <dory/serialization/jsonSerializer.h>
#include <dory/serialization/jsonDeserializer.h>

namespace dory::core::services::serialization
{
    template<typename T, typename TPolicy, typename TState>
    class YamlSerializerGeneric: public generic::implementation::ImplementationLevel<TPolicy, TState>
    {
    public:
        inline std::string serialize(T&& object) final
        {
            return dory::serialization::yaml::serialize(std::forward<T>(object));
        }

        inline T deserialize(const std::string& source) final
        {
            return dory::serialization::yaml::deserialize<T>(source);
        }

        inline void deserialize(const std::string& source, T& object) final
        {
            dory::serialization::yaml::deserialize(source, std::forward<T>(object));
        }
    };

    struct YamlSerializerPolicy: generic::implementation::ImplementationPolicy<generic::implementation::ImplementationList<YamlSerializerGeneric>>
    {};

    using YamlSerializer = generic::implementation::Implementation<generic::TypeList<ISerializer>, ISerializer::Types, YamlSerializerPolicy>;
}
