#pragma once

#include <variant>

#include "dory/serialization/yamlSerializer.h"
#include "dory/serialization/yamlDeserializer.h"
#include "dory/serialization/jsonSerializer.h"
#include "dory/serialization/jsonDeserializer.h"

namespace dory::engine::services::serialization
{
    template<typename TImplementation>
    class ISerializationService: NonCopyable, public StaticInterface<TImplementation>
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

    template<typename TFormat, typename TImplementation>
    class IFormatKeyConverter: NonCopyable, public StaticInterface<TImplementation>
    {
    public:
        TFormat getFormat(const std::string& key)
        {
            return this->toImplementation()->getFormatImpl(key);
        }

        TFormat getFormat(const std::string_view key)
        {
            return this->toImplementation()->getFormatImpl(key);
        }

        TFormat getFormat(const std::filesystem::path& path)
        {
            return this->toImplementation()->getFormatImpl(path);
        }
    };

    class FormatKeyConverter: public IFormatKeyConverter<Format, FormatKeyConverter>
    {
    private:
        constexpr const static char* yaml = "yaml";
        constexpr const static char* json = "json";

    public:
        using FormatType = Format;

        static Format getFormatImpl(const std::string& key)
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

        static Format getFormatImpl(const std::string_view key)
        {
            return getFormatImpl(std::string{key});
        }

        static Format getFormatImpl(const std::filesystem::path& path)
        {
            auto extension = std::string{ path.extension().string() };
            if(!extension.empty() && extension[0] == '.')
            {
                return getFormatImpl(std::string_view{ ++extension.begin(), extension.end() });
            }

            return getFormatImpl(extension);
        }
    };

    template<typename TFormat, typename TImplementation>
    class ISerializationServiceBundle: NonCopyable, public StaticInterface<TImplementation>
    {
    public:
        template<typename T>
        std::string serialize(const TFormat format, T&& object)
        {
            return this->toImplementation()->serializeImpl(format, std::forward<T>(object));
        }

        template<typename T>
        T deserialize(const TFormat format, const std::string& source)
        {
            return this->toImplementation()->deserializeImpl(format, source);
        }

        template<typename T>
        void deserialize(const TFormat format, const std::string& source, T& object)
        {
            this->toImplementation()->deserializeImpl(format, source, object);
        }
    };

    template<typename TFormat, typename... TSerializationServices>
    class SerializationServiceBundle: public ISerializationServiceBundle<TFormat, SerializationServiceBundle<TFormat, TSerializationServices...>>
    {
    public:
        using InitType = bool;
        template<typename T>
        using SerializationServiceRefType = std::reference_wrapper<ISerializationService<T>>;
        using SerializationServiceMapValueType = std::variant<InitType, SerializationServiceRefType<TSerializationServices>...>;
        using SerializationServiceMapType = std::map<TFormat, SerializationServiceMapValueType>;

    private:
        SerializationServiceMapType serializationServiceMap;

        decltype(auto) getSerializationService(const TFormat& format)
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
        std::string serializeImpl(const TFormat& format, T&& object)
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
        T deserializeImpl(const TFormat& format, const std::string& source)
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
        void deserializeImpl(const TFormat& format, const std::string& source, T& object)
        {
            auto serializationServiceOptional = getSerializationService(format);
            if(serializationServiceOptional)
            {
                std::visit([&](auto&& serviceRef)
                {
                    if constexpr (!std::is_same_v<InitType, std::decay_t<decltype(serviceRef)>>)
                    {
                        serviceRef.get().deserialize(source, object);
                    }
                }, *serializationServiceOptional);
            }
        }
    };
}