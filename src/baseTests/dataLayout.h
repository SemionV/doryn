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

    template <typename TAttributeId, class... TAttributes>
    struct Layout;

    template<typename TAttributeId>
    struct Layout<TAttributeId>
    {
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
    };
}