#pragma once

#include <string>
#include <dory/core/generic/typeList.h>

namespace dory::core::services::serialization
{
    template<typename T>
    class ISerializerGeneric
    {
    public:
        virtual std::string serialize(T&& object) = 0;
        virtual T deserialize(const std::string& source) = 0;
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

    using SerializationTypes = generic::TypeList<>;
    using ISerializer = ISerializerBundle<SerializationTypes>;
}
