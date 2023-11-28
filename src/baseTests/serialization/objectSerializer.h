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
            bool firstMember = true;

            for_each(refl::reflect(object).members, [&](auto member)
            {
                if constexpr (is_readable(member))
                {
                    using MemberDescriptorType = decltype(member);
                    auto& memberValue = object.*MemberDescriptorType::pointer;

                    context.processBeginMember((std::string)MemberDescriptorType::name, firstMember);

                    process(memberValue, context);

                    context.processEndMember();

                    firstMember = false;
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

        void processBeginMember(const std::string& memberName, bool firstMember)
        {
            if(!firstMember)
            {
                stream << "," << std::endl;
            }

            printIndent();
            stream << "\"" << memberName << "\"" << ": ";
        }

        void processEndMember()
        {
        }

        template<typename T>
        void processValue(T&& value)
        {
            stream << value;
            if(nestingLevel == 0)
            {
                stream << std::endl;
            }
        }

        template<typename T>
        void processBeginObject(T& object)
        {
            if(nestingLevel > 0)
            {
                stream << std::endl;
            }
            printIndent();
            stream << "{" << std::endl;
            ++nestingLevel;
        }

        void processEndObject()
        {
            --nestingLevel;
            stream << std::endl;
            printIndent();
            stream << "}";

            if(nestingLevel == 0)
            {
                stream << std::endl;
            }
        }

    private:
        void printIndent()
        {
            for(std::size_t i = 0; i < nestingLevel; ++i)
            {
                stream << "    ";
            }
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
