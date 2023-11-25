#pragma once

#include "baseTests/dependencies.h"

namespace dory::serialization
{
    class ObjectProcessor
    {
    private:
        template<template<typename> class TTrivialValuePolicy, typename TMemberPolicy, typename T>
        static std::size_t processCompoundObject(T&& object, Byte* buffer)
        {
            std::size_t offset = {};

            for_each(refl::reflect(object).members, [&](auto member)
            {
                if constexpr (is_readable(member))
                {
                    using MemberDescriptorType = decltype(member);
                    auto& memberValue = object.*MemberDescriptorType::pointer;

                    if constexpr (!std::is_same_v<TMemberPolicy, void>)
                    {
                        TMemberPolicy::process((std::string)MemberDescriptorType::name);
                    }

                    offset += process<TTrivialValuePolicy, TMemberPolicy>(memberValue, buffer + offset);
                }
            });

            return offset;
        }

    public:
        template<template<typename> class TTrivialValuePolicy, typename TMemberPolicy = void, typename T>
        static std::size_t process(T&& object, Byte* buffer)
        {
            std::size_t size;

            using ValueType = std::remove_reference_t<T>;

            if constexpr (std::is_trivial_v<ValueType>)
            {
                size = sizeof(object);
                TTrivialValuePolicy<T>::process(std::forward<T>(object), size, buffer);
            }
            else
            {
                size = processCompoundObject<TTrivialValuePolicy, TMemberPolicy>(std::forward<T>(object), buffer);
            }

            return size;
        }
    };

    template<typename T>
    struct WriteValueBinary
    {
        static void process(T&& value, std::size_t size, Byte* buffer)
        {
            memcpy(buffer, &value, size);
        }
    };

    template<typename T>
    struct ReadValueBinary
    {
        static void process(T&& value, std::size_t size, Byte* buffer)
        {
            memcpy(&value, buffer, size);
        }
    };

    class ObjectBinarySerializer
    {
        
    };

    struct PrintMemberName
    {
        static void process(const std::string& memberName)
        {
            std::cout << memberName << ": ";
        }
    };

    template<typename T>
    struct PrintValue
    {
        static void process(T&& value, std::size_t size, Byte* buffer)
        {
            std::cout << value << std::endl;
        }
    };

    class ObjectPrinter
    {
    public:
        template<typename T>
        static void print(T&& object)
        {
            ObjectProcessor::process<PrintValue, PrintMemberName>(std::forward<T>(object), nullptr);
        }
    };
}
