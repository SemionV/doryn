#pragma once

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

    template<typename TAttributeId, typename TAttribute, typename... TAttributes>
    struct Layout<TAttributeId, TAttribute, TAttributes...>: public Layout<TAttributeId, TAttributes...>
    {
        using ParentType = Layout<TAttributeId, TAttributes...>;

        static constexpr std::size_t getAttributeSize()
        {
            return sizeof(typename TAttribute::type);
        }

        static constexpr std::size_t getSize()
        {
            if constexpr (sizeof...(TAttributes) > 0)
            {
                return getAttributeSize() + ParentType::getSize();
            }

            return getAttributeSize();
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
    };
}