#pragma once

#include "dependencies.h"

namespace dory
{
    template<typename TLeft, typename TRight>
    struct TypePair
    {
        using LeftType = TLeft;
        using RightType = TRight;
    };

    template<typename TTypePair, typename TConverter = void>
    struct TypeAssigment
    {
        using TypePairType = TTypePair;
        using ConverterType = TConverter;
    };

    template<typename... TTypeAssignments>
    struct TypeMap
    {
    };

    template<typename T, typename TMap>
    struct MappedTypeToRight;

    template<typename T, typename TLeft, typename TRight, typename TConverter, typename... TLefts, typename... TRights, typename... TConverters>
    struct MappedTypeToRight<T, TypeMap<TypeAssigment<TypePair<TLeft, TRight>, TConverter>,  TypeAssigment<TypePair<TLefts, TRights>, TConverters>...>>
    {
        using Type = std::conditional_t<std::is_same_v<TLeft, T>,
            TRight,
            typename MappedTypeToRight<T, TypeMap<TypeAssigment<TypePair<TLefts, TRights>, TConverters>...>>::Type>;
    };

    template<typename T>
    struct MappedTypeToRight<T, TypeMap<>>
    {
        using Type = T;
    };

    template<typename T, typename TMap>
    using MappedTypeToRightT = typename MappedTypeToRight<T, TMap>::Type;
}