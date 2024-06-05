#pragma once

#include "base/dependencies.h"
#include "base/typeComponents.h"
#include "base/serialization/yamlDeserializer.h"
#include "base/serialization/yamlSerializer.h"
#include "base/serialization/jsonDeserializer.h"
#include "base/serialization/jsonSerializer.h"

namespace dory::domain::services
{
    template<typename TImplementation>
    class ISerializationService: Uncopyable, public StaticInterface<TImplementation>
    {
    public:
        template<typename T>
        std::string serialize(T&& object)
        {
            return this->toImplementation()->serializeImpl(std::forward<T>(object));
        }

        template<typename T>
        T deserialize(const std::string& source)
        {
            return this->toImplementation()->deserializeImpl(source);
        }

        template<typename T>
        void deserialize(const std::string& source, T& object)
        {
            this->toImplementation()->deserializeImpl(source, object);
        }
    };

    class YamlSerializationService: public ISerializationService<YamlSerializationService>
    {
    public:
        template<typename T>
        static std::string serializeImpl(T&& object)
        {
            return serialization::yaml::serialize(std::forward<T>(object));
        }

        template<typename T>
        static T deserializeImpl(const std::string& source)
        {
            return serialization::yaml::deserialize<T>(source);
        }

        template<typename T>
        static void deserializeImpl(const std::string& source, T&& object)
        {
            serialization::yaml::deserialize(source, std::forward<T>(object));
        }
    };

    template<int ident = -1>
    class JsonSerializationService: public ISerializationService<JsonSerializationService<ident>>
    {
    public:
        template<typename T>
        static std::string serializeImpl(T&& object)
        {
            return serialization::json::serialize(std::forward<T>(object), ident);
        }

        template<typename T>
        static T deserializeImpl(const std::string& source)
        {
            return serialization::json::deserialize<T>(source);
        }

        template<typename T>
        static void deserializeImpl(const std::string& source, T&& object)
        {
            serialization::json::deserialize(source, std::forward<T>(object));
        }
    };
}