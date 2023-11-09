#pragma once

#include "dependencies.h"

namespace dory
{
    template<typename TAttributeId, TAttributeId Id, typename T>
    struct Attribute
    {
        static constexpr TAttributeId id = Id;
        using type = T;
    };

    template<typename T>
    struct ValueType
    {
        using type = T;
    };

    template <typename TAttributeId, class... TAttributes>
    struct Layout;

    template<typename TAttributeId>
    struct Layout<TAttributeId>
    {
    };

    template<typename TAttributeId, typename T, typename TMemberValueType, TAttributeId Id, std::size_t Size, std::size_t Offset, std::size_t MembersCount>
    struct AttributeDescriptor
    {
        using type = T;
        using memberValueType = TMemberValueType;
        static const std::size_t size = Size;
        static const std::size_t offset = Offset;
        static const std::size_t membersCount = MembersCount;
        static const TAttributeId id = Id;
    };

    template<typename T>
    constexpr std::size_t getReflTypeSize()
    {
        std::size_t size {};

        if constexpr (std::is_trivial_v<T>)
        {
            size = sizeof(T);
        }
        else
        {
            constexpr refl::descriptor::type_descriptor<std::remove_reference_t<T>> typeDescriptor{};

            for_each(typeDescriptor.members, [&](auto member)
            {
                if constexpr (is_readable(member))
                {
                    using memberType = decltype(member);

                    if constexpr (std::is_trivial_v<typename memberType::value_type>)
                    {
                        size += sizeof(typename memberType::value_type);
                    }
                    else
                    {
                        size += getReflTypeSize<typename memberType::value_type>();
                    }
                    
                }
            });
        }

        return size;
    }

    template<typename T>
    constexpr std::size_t getReflTypeMembersCount()
    {
        std::size_t count {};

        if constexpr (std::is_trivial_v<T>)
        {
            count = 1;
        }
        else
        {
            constexpr refl::descriptor::type_descriptor<std::remove_reference_t<T>> typeDescriptor{};

            for_each(typeDescriptor.members, [&](auto member)
            {
                if constexpr (is_readable(member))
                {
                    using memberType = decltype(member);

                    if constexpr (std::is_trivial_v<typename memberType::value_type>)
                    {
                        ++count;
                    }
                    else
                    {
                        count += getReflTypeMembersCount<typename memberType::value_type>();
                    }
                    
                }
            });
        }

        return count;
    }

    template<typename T, std::size_t memberCount, std::size_t i = 0>
    constexpr decltype(auto) getReflComplexTypeMembersCommonType()
    {
        if constexpr (i < memberCount)
        {
            using TypeInfo = refl::detail::type_info<T>;
            using MemberValueType = typename refl::detail::member_info<T, i>::value_type;

            if constexpr (std::is_trivial_v<MemberValueType>)
            {
                return ValueType<MemberValueType>();
            }
            else
            {
                return getReflComplexTypeMembersCommonType<T, memberCount, i + 1>();
            }
        }
        else
        {
            return ValueType<void>();
        }
    }

    template<typename T>
    constexpr decltype(auto) getReflTypeMembersCommonType()
    {
        if constexpr (std::is_trivial_v<T>)
        {
            return ValueType<T>();
        }
        else
        {
            using TypeInfo = refl::detail::type_info<std::remove_reference_t<T>>;
            return getReflComplexTypeMembersCommonType<T, TypeInfo::member_count>();
        }
    }

    template<typename TAttributeId, typename TAttribute, typename... TAttributes>
    struct Layout<TAttributeId, TAttribute, TAttributes...>: public Layout<TAttributeId, TAttributes...>
    {
        using ParentType = Layout<TAttributeId, TAttributes...>;

        static constexpr std::size_t getAttributeSize()
        {
            return getReflTypeSize<typename TAttribute::type>();
        }

        template<TAttributeId attributeId>
        static constexpr std::size_t getAttributeSize()
        {
            if constexpr(TAttribute::id == attributeId)
            {
                return getAttributeSize();
            }
            else if constexpr (sizeof...(TAttributes) > 0)
            {
                return ParentType::template getAttributeSize<attributeId>();
            }
            else
            {
                static_assert(sizeof...(TAttributes) == 0, "Invalid attribute id");
            }
        }

        static constexpr std::size_t getSize()
        {
            if constexpr (sizeof...(TAttributes) > 0)
            {
                return getAttributeSize() + ParentType::getSize();
            }

            return getAttributeSize();
        }

        static constexpr std::size_t getCount()
        {
            std::size_t count = 1;

            if constexpr(sizeof...(TAttributes) > 0)
            {
                count += ParentType::getCount();
            }

            return count;
        }

        template<TAttributeId attributeId, std::size_t offset = 0>
        static constexpr std::size_t getAttributeOffset()
        {
            if constexpr(TAttribute::id == attributeId)
            {
                return offset;
            }
            else if constexpr (sizeof...(TAttributes) == 1)
            {
                return offset + getAttributeSize();
            }
            else if constexpr(sizeof...(TAttributes) > 1)
            {
                return ParentType::template getAttributeOffset<attributeId, offset + getAttributeSize()>();
            }
            else
            {
                static_assert(sizeof...(TAttributes) == 0, "Invalid attribute id");
            }
        }

        template<TAttributeId attributeId>
        static constexpr decltype(auto) getAttributeTypeDescriptor()
        {
            if constexpr(TAttribute::id == attributeId)
            {
                return refl::descriptor::type_descriptor<typename TAttribute::type> {};
            }
            else if constexpr (sizeof...(TAttributes) > 0)
            {
                return ParentType::template getAttributeTypeDescriptor<attributeId>();
            }
            else
            {
                static_assert(sizeof...(TAttributes) == 0, "Invalid attribute id");
            }
        }

        template<TAttributeId attributeId, std::size_t offset = 0>
        static constexpr decltype(auto) getDescriptor()
        {
            constexpr std::size_t parentAttributesCount = sizeof...(TAttributes);
            if constexpr (attributeId == TAttribute::id)
            {
                auto membersValueType = getReflTypeMembersCommonType<typename TAttribute::type>();
                using MembersValueType = decltype(membersValueType);
                return AttributeDescriptor<TAttributeId, 
                    typename TAttribute::type, 
                    typename MembersValueType::type, 
                    TAttribute::id, 
                    getAttributeSize(), 
                    offset,
                    getReflTypeMembersCount<typename TAttribute::type>()
                >();
            }
            else if constexpr (sizeof...(TAttributes) > 0)
            {
                return ParentType::template getDescriptor<attributeId, offset + getAttributeSize()>();
            }
            else
            {
                static_assert(sizeof...(TAttributes) == 0, "Invalid attribute id");
            }
        }
    };
}