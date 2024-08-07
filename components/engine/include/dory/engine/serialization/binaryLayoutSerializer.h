#pragma once

#include "objectVisitor.h"
#include "structures/dataLayout.h"

namespace dory::serialization
{
    struct BinarySerializationContext
    {
        Byte* buffer;
        std::size_t offset {};

        explicit BinarySerializationContext(Byte* buffer):
                buffer(buffer) {}
    };

    struct WriteBinaryValuePolicy
    {
        template<typename T, typename TContext>
        inline static void process(T& value, TContext& context)
        {
            auto size = sizeof(T);
            memcpy(context.buffer + context.offset, &value, size);
            context.offset += size;
        }
    };

    struct ReadBinaryValuePolicy
    {
        template<typename T, typename TContext>
        inline static void process(T& value, TContext& context)
        {
            auto size = sizeof(T);
            memcpy(&value, context.buffer + context.offset, size);
            context.offset += size;
        }
    };

    struct WriteBinaryPolicies: public dory::serialization::VisitorDefaultPolicies
    {
        using ValuePolicy = WriteBinaryValuePolicy;
    };

    struct ReadBinaryPolicies: public dory::serialization::VisitorDefaultPolicies
    {
        using ValuePolicy = ReadBinaryValuePolicy;
    };

    template<typename TLayout>
    class BinaryLayoutSerializer
    {
    private:
        template<auto Id, typename TPolicies, typename T>
        static std::size_t processAttribute(T&& attributeValue, Byte* buffer)
        {
            auto offset = LayoutAttributeOffsetV<Id, TLayout>;
            BinarySerializationContext context(buffer + offset);
            dory::serialization::ObjectVisitor<TPolicies>::visit(std::forward<T>(attributeValue), context);

            return context.offset;
        }

    public:
        template<auto Id, typename T>
        static std::size_t writeAttribute(const T& attributeValue, Byte* buffer)
        {
            return processAttribute<Id, WriteBinaryPolicies>(attributeValue, buffer);
        }

        template<auto Id, typename T>
        static std::size_t readAttribute(T& attributeValue, Byte* buffer)
        {
            return processAttribute<Id, ReadBinaryPolicies>(attributeValue, buffer);
        }
    };
}