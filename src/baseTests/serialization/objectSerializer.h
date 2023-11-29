#pragma once

#include "baseTests/dependencies.h"

namespace dory::serialization
{
    struct DefaultValuePolicy
    {
        template<typename T, typename TContext>
        inline static void processValue(T&& value, TContext& context)
        {
        }
    };

    struct VisitorDefaultPolicies
    {
        using ValuePolicy = DefaultValuePolicy;
    };

    template<typename TPolicies = VisitorDefaultPolicies>
    class ObjectVisitor
    {
    private:
        template<typename T>
        static constexpr std::size_t getClassMemberCount(refl::descriptor::type_descriptor<T> typeDescriptor)
        {
            std::size_t count {};
            visitClassMembers(typeDescriptor, [&count](auto memberDescriptor) {
                ++count;
            });

            return count;
        }

        template<typename T, typename F>
        static constexpr void visitClassMembers(refl::descriptor::type_descriptor<T> typeDescriptor, F functor)
        {
            for_each(typeDescriptor.members, [&](auto memberDescriptor)
            {
                //so far only for fields, possible to add support for member functions
                if constexpr (is_readable(memberDescriptor) && is_field(memberDescriptor))
                {
                    functor(memberDescriptor);
                }
            });
        }

        template<typename T, typename TContext>
        static void visitClassMember(T& memberValue, const std::string& memberName, const std::size_t i, const std::size_t memberCount, TContext& context)
        {
            context.processBeginMember(memberName, i);
            visit(memberValue, context);
            context.processEndMember(i == memberCount - 1);
        }

        template<typename T, auto N, typename TContext>
        static void visitArray(std::array<T, N>& object, TContext& context)
        {
            context.processBeginCollection();

            for(std::size_t i {}; i < N; ++i)
            {
                context.processBeginCollectionItem(i);
                visit(object[i], context);
                context.processEndCollectionItem(i == N - 1);
            }

            context.processEndCollection();
        }

    public:
        template<typename T, typename TContext>
        requires(std::is_trivial_v<std::remove_reference_t<T>>)
        static void visit(T&& object, TContext& context)
        {
            TPolicies::ValuePolicy::processValue(std::forward<T>(object), context);
        }

        template<typename T, typename TContext>
        requires(std::is_class_v<std::remove_reference_t<T>>)
        static void visit(T&& object, TContext& context)
        {
            context.processBeginObject(object);
            auto constexpr typeDescriptor = refl::descriptor::type_descriptor<std::remove_reference_t<T>>{};
            const constexpr auto memberCount = getClassMemberCount(typeDescriptor);
            std::size_t i = {};

            visitClassMembers(typeDescriptor, [&object, &context, &i, memberCount](auto memberDescriptor) {
                if constexpr (is_field(memberDescriptor))
                {
                    using MemberDescriptorType = decltype(memberDescriptor);

                    auto& memberValue = object.*MemberDescriptorType::pointer;
                    const auto& memberName = (std::string) MemberDescriptorType::name;
                    visitClassMember(memberValue, memberName, i, memberCount, context);
                }

                ++i;
            });

            context.processEndObject();
        }

        template<typename T, auto N, typename TContext>
        static void visit(std::array<T, N>& object, TContext& context)
        {
            visitArray(object, context);
        }

        template<typename T, auto N, typename TContext>
        static void visit(std::array<T, N>&& object, TContext& context)
        {
            visitArray(object, context);
        }
    };

    class ObjectVisitorDefaultPolicy
    {
    public:
        void processBeginCollection()
        {
        }

        void processEndCollection()
        {
        }

        void processBeginCollectionItem(std::size_t i)
        {
        }

        void processEndCollectionItem(bool lastItem)
        {
        }

        void processBeginMember(const std::string& memberName, std::size_t i)
        {
        }

        void processEndMember(bool lastMember)
        {
        }

        template<typename T>
        void processBeginObject(T& object)
        {
        }

        void processEndObject()
        {
        }
    };

    class ObjectJsonPrintingPolicy: public ObjectVisitorDefaultPolicy
    {
    public:
        std::ostream& stream;
        unsigned int nestingLevel {};

    public:
        explicit ObjectJsonPrintingPolicy(std::ostream &stream) :
                stream(stream) {}

        void processBeginCollection()
        {
            stream << "[" << std::endl;
            ++nestingLevel;
        }

        void processEndCollection()
        {
            --nestingLevel;
            printIndent();
            stream << "]";
        }

        void processBeginCollectionItem(std::size_t i)
        {
            printIndent();
        }

        void processEndCollectionItem(bool lastItem)
        {
            if(!lastItem)
            {
                stream << ",";
            }

            stream << std::endl;
        }

        void processBeginMember(const std::string& memberName, std::size_t i)
        {
            printIndent();
            stream << "\"" << memberName << "\"" << ": ";
        }

        void processEndMember(bool lastMember)
        {
            if(!lastMember)
            {
                stream << ",";
            }

            stream << std::endl;
        }

        template<typename T>
        void processBeginObject(T& object)
        {
            stream << "{" << std::endl;
            ++nestingLevel;
        }

        void processEndObject()
        {
            --nestingLevel;
            printIndent();
            stream << "}";
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

    struct PrintValuePolicy
    {
        template<typename T, typename TContext>
        inline static void processValue(T&& value, TContext& context)
        {
            context.stream << value;
        }
    };

    struct PrintingPolicies: public VisitorDefaultPolicies
    {
        using ValuePolicy = PrintValuePolicy;
    };

    class ObjectPrinter
    {
    public:
        template<typename T>
        static void print(T&& object)
        {
            ObjectJsonPrintingPolicy printingPolicy(std::cout);

            ObjectVisitor<PrintingPolicies>::visit(std::forward<T>(object), printingPolicy);

            std::cout << std::endl;
        }
    };
}
