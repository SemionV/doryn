#pragma once

#include "templates/objectVisitor.h"

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

    struct PrintBeginObjectPolicy
    {
        template<typename TContext>
        inline static void process(TContext& context)
        {
            context.stream << "{" << std::endl;
            ++context.nestingLevel;
        }
    };

    struct PrintEndObjectPolicy
    {
        template<typename TContext>
        inline static void process(TContext& context)
        {
            --context.nestingLevel;
            JsonFormatting::printIndent(context);
            context.stream << "}";
        }
    };

    struct PrintBeginMemberPolicy
    {
        template<typename TContext>
        inline static void process(const std::string& memberName, const std::size_t i, TContext& context)
        {
            JsonFormatting::printIndent(context);
            context.stream << "\"" << memberName << "\"" << ": ";
        }
    };

    struct PrintEndMemberPolicy
    {
        template<typename TContext>
        inline static void process(const bool lastMember, TContext& context)
        {
            if(!lastMember)
            {
                context.stream << ",";
            }

            context.stream << std::endl;
        }
    };

    struct PrintBeginCollectionPolicy
    {
        template<typename T, auto N, typename TContext>
        inline static void process(TContext& context)
        {
            context.stream << "[" << std::endl;
            ++context.nestingLevel;
        }
    };

    struct PrintEndCollectionPolicy
    {
        template<typename TContext>
        inline static void process(TContext& context)
        {
            --context.nestingLevel;
            JsonFormatting::printIndent(context);
            context.stream << "]";
        }
    };

    struct PrintBeginCollectionItemPolicy
    {
        template<typename TContext>
        inline static void process(const std::size_t i, TContext& context)
        {
            JsonFormatting::printIndent(context);
        }
    };

    struct PrintEndCollectionItemPolicy
    {
        template<typename TContext>
        inline static void process(const bool lastItem, TContext& context)
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
        using BeginMemberPolicy = PrintBeginMemberPolicy;
        using EndMemberPolicy = PrintEndMemberPolicy;
        using BeginCollectionPolicy = PrintBeginCollectionPolicy;
        using EndCollectionPolicy = PrintEndCollectionPolicy;
        using BeginCollectionItemPolicy = PrintBeginCollectionItemPolicy;
        using EndCollectionItemPolicy = PrintEndCollectionItemPolicy;
        using BeginObjectPolicy = PrintBeginObjectPolicy;
        using EndObjectPolicy = PrintEndObjectPolicy;
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
