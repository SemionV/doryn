#pragma once

#include "objectVisitor.h"

namespace dory::typeMap
{
    struct JsonPrintingContext
    {
        std::ostream& stream;
        unsigned int nestingLevel {};

        explicit JsonPrintingContext(std::ostream &stream) :
                stream(stream) {}
    };

    struct JsonFormatting
    {
        template<typename TContext>
        inline static void printIndent(TContext& context)
        {
            for(std::size_t i = 0; i < context.nestingLevel; ++i)
            {
                context.stream << "    ";
            }
        }
    };

    struct PrintValuePolicy
    {
        template<typename T, typename TContext>
        inline static void process(T&& value, TContext& context)
        {
            context.stream << value;
        }
    };

    struct PrintObjectPolicy
    {
        template<typename TContext>
        inline static void beginObject(TContext& context)
        {
            context.stream << "{" << std::endl;
            ++context.nestingLevel;
        }

        template<typename TContext>
        inline static void endObject(TContext& context)
        {
            --context.nestingLevel;
            JsonFormatting::printIndent(context);
            context.stream << "}";
        }
    };

    struct PrintMemberPolicy
    {
        template<typename TContext>
        inline static void beginMember(const std::string& memberName, const std::size_t i, TContext& context)
        {
            JsonFormatting::printIndent(context);
            context.stream << "\"" << memberName << "\"" << ": ";
        }

        template<typename TContext>
        inline static void endMember(const bool lastMember, TContext& context)
        {
            if(!lastMember)
            {
                context.stream << ",";
            }

            context.stream << std::endl;
        }
    };

    struct PrintCollectionPolicy
    {
        template<typename T, auto N, typename TContext>
        inline static void beginCollection(TContext& context)
        {
            context.stream << "[" << std::endl;
            ++context.nestingLevel;
        }

        template<typename TContext>
        inline static void endCollection(TContext& context)
        {
            --context.nestingLevel;
            JsonFormatting::printIndent(context);
            context.stream << "]";
        }
    };

    struct PrintCollectionItemPolicy
    {
        template<typename TContext>
        inline static void beginItem(const std::size_t i, TContext& context)
        {
            JsonFormatting::printIndent(context);
        }

        template<typename TContext>
        inline static void endItem(const bool lastItem, TContext& context)
        {
            if(!lastItem)
            {
                context.stream << ",";
            }

            context.stream << std::endl;
        }
    };

    struct PrintingPolicies: public VisitorDefaultPolicies
    {
        using ValuePolicy = PrintValuePolicy;
        using ObjectPolicy = PrintObjectPolicy;
        using MemberPolicy = PrintMemberPolicy;
        using CollectionPolicy = PrintCollectionPolicy;
        using CollectionItemPolicy = PrintCollectionItemPolicy;
    };

    class ObjectPrinter
    {
    public:
        template<typename T>
        static void print(T&& object)
        {
            JsonPrintingContext context(std::cout);
            ObjectVisitor<PrintingPolicies>::visit(std::forward<T>(object), context);

            std::cout << std::endl;
        }
    };
}
