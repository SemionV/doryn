#pragma once

namespace dory::serialization
{
    template<typename TLayout>
    class ObjectProcessor
    {
    private:
        template<typename T, typename TDescriptor, template<typename, typename> class TTrivialMemberPolicy>
        static std::size_t processComplexValue(const T& attributeValue, Byte* buffer, TDescriptor typeDescriptor)
        {
            std::size_t offset = {};

            for_each(typeDescriptor.members, [&](auto member)
            {
                if constexpr (is_readable(member))
                {
                    using MemberDescriptorType = decltype(member);

                    if constexpr (std::is_trivial_v<typename MemberDescriptorType::value_type>)
                    {
                        offset += TTrivialMemberPolicy<T, MemberDescriptorType>::process(attributeValue, member);
                    }
                    else
                    {
                        refl::descriptor::type_descriptor<typename MemberDescriptorType::value_type> typeDescriptor{};
                        offset += processComplexValue(member(attributeValue), buffer + offset, typeDescriptor);
                    }
                }
            });

            return offset;
        }
    };

    template<typename T, typename TMemberDescriptor>
    struct WriteTrivialMemberBinary
    {
        static std::size_t process(const T& object, TMemberDescriptor& memberDescriptor, Byte* buffer)
        {
            auto size = sizeof(typename TMemberDescriptor::value_type);
            memcpy(buffer, &memberDescriptor(object), size);
            return size;
        }
    };

    template<typename T, typename TMemberDescriptor>
    struct ReadTrivialMemberBinary
    {
        static std::size_t process(const T& object, TMemberDescriptor& memberDescriptor, Byte* buffer)
        {
            auto size = sizeof(typename TMemberDescriptor::value_type);
            memcpy(&memberDescriptor(object), buffer, size);
            return size;
        }
    };

    class ObjectSerializerBinary
    {
        
    };
}
