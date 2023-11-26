#pragma once

#include "baseTests/dependencies.h"

namespace dory::serialization
{
    class ObjectProcessor
    {
    private:
        template<template<typename> class TTrivialValuePolicy, typename TMemberPolicy, typename T, typename TContext>
        static void processCompoundObject(T&& object, TContext& context)
        {
            for_each(refl::reflect(object).members, [&](auto member)
            {
                if constexpr (is_readable(member))
                {
                    using MemberDescriptorType = decltype(member);
                    auto& memberValue = object.*MemberDescriptorType::pointer;

                    if constexpr (!std::is_same_v<TMemberPolicy, void>)
                    {
                        TMemberPolicy::process((std::string)MemberDescriptorType::name, context);
                    }

                    process<TTrivialValuePolicy, TMemberPolicy>(memberValue, context);
                }
            });
        }

    public:
        template<template<typename> class TTrivialValuePolicy, typename TMemberPolicy = void, typename T, typename TContext>
        static void process(T&& object, TContext& context)
        {
            using ValueType = std::remove_reference_t<T>;

            if constexpr (std::is_trivial_v<ValueType>)
            {
                TTrivialValuePolicy<T>::process(std::forward<T>(object), context);
            }
            else
            {
                processCompoundObject<TTrivialValuePolicy, TMemberPolicy>(std::forward<T>(object), context);
            }
        }
    };

    struct BufferContext
    {
        Byte* buffer;
    };

    template<typename T>
    struct WriteValueBinary
    {
        static void process(T&& value, BufferContext& context)
        {
            auto size = sizeof(value);
            memcpy(context.buffer, &value, size);
            context.buffer += size;
        }
    };

    template<typename T>
    struct ReadValueBinary
    {
        static void process(T&& value, BufferContext& context)
        {
            auto size = sizeof(value);
            memcpy(&value, context.buffer, size);
            context.buffer += size;
        }
    };

    struct PrintMemberName
    {
        static void process(const std::string& memberName, std::ostream& stream)
        {
            stream << memberName << ": ";
        }
    };

    template<typename T>
    struct PrintValue
    {
        static void process(T&& value, std::ostream& stream)
        {
            stream << value << std::endl;
        }
    };

    class ObjectPrinter
    {
    public:
        template<typename T>
        static void print(T&& object)
        {
            ObjectProcessor::process<PrintValue, PrintMemberName>(std::forward<T>(object), std::cout);
        }
    };
}
