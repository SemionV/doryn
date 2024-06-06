#pragma once

#include "base/dependencies.h"
#include "base/typeComponents.h"
#include "base/serialization/yamlDeserializer.h"
#include "base/serialization/yamlSerializer.h"
#include "base/serialization/jsonDeserializer.h"
#include "base/serialization/jsonSerializer.h"

namespace dory::domain::services::serialization
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
            return dory::serialization::yaml::serialize(std::forward<T>(object));
        }

        template<typename T>
        static T deserializeImpl(const std::string& source)
        {
            return dory::serialization::yaml::deserialize<T>(source);
        }

        template<typename T>
        static void deserializeImpl(const std::string& source, T&& object)
        {
            dory::serialization::yaml::deserialize(source, std::forward<T>(object));
        }
    };

    template<int ident = -1>
    class JsonSerializationService: public ISerializationService<JsonSerializationService<ident>>
    {
    public:
        template<typename T>
        static std::string serializeImpl(T&& object)
        {
            return dory::serialization::json::serialize(std::forward<T>(object), ident);
        }

        template<typename T>
        static T deserializeImpl(const std::string& source)
        {
            return dory::serialization::json::deserialize<T>(source);
        }

        template<typename T>
        static void deserializeImpl(const std::string& source, T&& object)
        {
            dory::serialization::json::deserialize(source, std::forward<T>(object));
        }
    };

    enum class Format
    {
        unknown,
        yaml,
        json
    };

    struct FormatKey
    {
        constexpr const static std::string yaml = "yaml";
        constexpr const static std::string json = "json";

        static Format getFormat(const std::string& key)
        {
            auto lowerCaseKey = dory::toLower(key);
            if(lowerCaseKey == yaml)
            {
                return Format::yaml;
            }
            else if(lowerCaseKey == json)
            {
                return Format::json;
            }

            return Format::unknown;
        }

        static Format getFormat(const std::string_view key)
        {
            return getFormat(std::string{key});
        }

        static Format getFormat(const std::filesystem::path& path)
        {
            std::string extension = path.extension();
            if(!extension.empty() && extension[0] == '.')
            {
                return getFormat(std::string_view{ ++extension.begin(), extension.end() });
            }

            return getFormat(extension);
        }
    };

    template<typename TImplementation>
    class ISerializationServiceBundle: Uncopyable, public StaticInterface<TImplementation>
    {
    public:
        template<typename T>
        std::string serialize(const Format format, T&& object)
        {
            return this->toImplementation()->serializeImpl(format, std::forward<T>(object));
        }

        template<typename T>
        T deserialize(const Format format, const std::string& source)
        {
            return this->toImplementation()->deserializeImpl(format, source);
        }

        template<typename T>
        void deserialize(const Format format, const std::string& source, T& object)
        {
            this->toImplementation()->deserializeImpl(format, source, object);
        }
    };

    template<typename... TSerializationServices>
    class SerializationServiceBundle: public ISerializationServiceBundle<SerializationServiceBundle<TSerializationServices...>>
    {
    public:
        template<typename T>
        using SerializationServiceRefType = std::reference_wrapper<ISerializationService<T>>;
        using SerializationServiceMapValueType = std::variant<bool, SerializationServiceRefType<TSerializationServices>...>;
        using SerializationServiceMapType = std::map<Format, SerializationServiceMapValueType>;

    private:
        SerializationServiceMapType serializationServiceMap;

        decltype(auto) getSerializationService(const Format format)
        {
            if(serializationServiceMap.contains(format))
            {
                return std::optional<SerializationServiceMapValueType>{ serializationServiceMap[format] };
            }
            return std::optional<SerializationServiceMapValueType>{};
        }

    public:
        explicit SerializationServiceBundle(SerializationServiceMapType serializationServiceMap):
                serializationServiceMap(std::move(serializationServiceMap))
        {}

        template<typename T>
        std::string serializeImpl(const Format format, T&& object)
        {
            auto serializationServiceOptional = getSerializationService(format);
            if(serializationServiceOptional)
            {
                auto content = std::string{};

                std::visit([&](auto&& serviceRef)
                {
                    if constexpr (!std::is_same_v<bool, std::decay_t<decltype(serviceRef)>>)
                    {
                        content = serviceRef.get().serialize(std::forward<T>(object));
                    }
                }, *serializationServiceOptional);

                return content;
            }

            return {};
        }

        template<typename T>
        T deserializeImpl(const Format format, const std::string& source)
        {
            auto serializationServiceOptional = getSerializationService(format);
            if(serializationServiceOptional)
            {
                T object;

                std::visit([&](auto&& serviceRef)
                {
                    if constexpr (!std::is_same_v<bool, std::decay_t<decltype(serviceRef)>>)
                    {
                        object = serviceRef.get().deserialize(source);
                    }
                }, *serializationServiceOptional);

                return object;
            }

            return {};
        }

        template<typename T>
        void deserializeImpl(const Format format, const std::string& source, T& object)
        {
            auto serializationServiceOptional = getSerializationService(format);
            if(serializationServiceOptional)
            {
                std::visit([&](auto&& serviceRef)
                {
                    if constexpr (!std::is_same_v<bool, std::decay_t<decltype(serviceRef)>>)
                    {
                        serviceRef.get().deserialize(source, object);
                    }
                }, *serializationServiceOptional);
            }
        }
    };
}