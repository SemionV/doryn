#pragma once

#include "baseTests/dependencies.h"

namespace dory::serialization
{
    class ObjectProcessor
    {
    private:
        template<typename T, typename TContext>
        static void processCompoundObject(T&& object, TContext& context)
        {
            context.processBeginObject(object);

            for_each(refl::reflect(object).members, [&](auto member)
            {
                if constexpr (is_readable(member))
                {
                    using MemberDescriptorType = decltype(member);
                    auto& memberValue = object.*MemberDescriptorType::pointer;

                    context.processMemberName((std::string)MemberDescriptorType::name);

                    process(memberValue, context);
                }
            });

            context.processEndObject();
        }

    public:
        template<typename T, typename TContext>
        static void process(T&& object, TContext& context)
        {
            using ValueType = std::remove_reference_t<T>;

            if constexpr (std::is_trivial_v<ValueType>)
            {
                context.processValue(std::forward<T>(object));
            }
            else
            {
                processCompoundObject(std::forward<T>(object), context);
            }
        }
    };

    class ObjectPrintingProcessor
    {
    private:
        std::ostream& stream;
        unsigned int nestingLevel {};

    public:
        explicit ObjectPrintingProcessor(std::ostream &stream) :
                stream(stream) {}

        void processMemberName(const std::string& memberName)
        {
            for(std::size_t i = 0; i < nestingLevel - 1; ++i)
            {
                stream << "    ";
            }
            stream << memberName << ": ";
        }

        template<typename T>
        void processValue(T&& value)
        {
            stream << value << std::endl;
        }

        template<typename T>
        void processBeginObject(T& object)
        {
            ++nestingLevel;
            stream << std::endl;
        }

        void processEndObject()
        {
            --nestingLevel;
        }
    };

    class ObjectPrinter
    {
    public:
        template<typename T>
        static void print(T&& object)
        {
            ObjectPrintingProcessor printingProcessor(std::cout);

            ObjectProcessor::process(std::forward<T>(object), printingProcessor);
        }
    };
}
