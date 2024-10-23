#pragma once

#include <string>
#include <dory/core/generic/typeList.h>

namespace dory::core::services::serialization
{
    template<typename T>
    class ISerializationServiceGeneric
    {
    public:
        virtual std::string serialize(T&& object) = 0;
        virtual T deserialize(const std::string& source) = 0;
        virtual void deserialize(const std::string& source, T& object) = 0;
    };

    template<typename... Ts>
    class ISerializationServiceBundle: public ISerializationServiceGeneric<Ts>...
    {
    public:
        using Types =  generic::TypeList<Ts...>;

        using ISerializationServiceGeneric<Ts>::serialize...;
        using ISerializationServiceGeneric<Ts>::deserialize...;

        virtual ~ISerializationServiceBundle() = default;
    };

    template<typename... Ts>
    class ISerializationServiceBundle<generic::TypeList<Ts...>>: public ISerializationServiceBundle<Ts...>
    {};

    using SerializationTypes = generic::TypeList<>;
    using ISerializationService = ISerializationServiceBundle<SerializationTypes>;
}
