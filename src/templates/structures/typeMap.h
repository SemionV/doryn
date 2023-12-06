#pragma once

namespace dory::typeMap
{
    template<typename T>
    struct Identity
    {
        using Type = T;
    };

    template<typename T>
    using IdentityT = Identity<T>;

    template<typename TKey, typename T>
    struct TypePair
    {
    };

    template<typename TTypePair, typename TForwardConverter = void, typename TBackwardConverter = void>
    struct TypeAssigment
    {
    };

    template<typename... TTypeAssignments>
    struct TypeMap
    {
    };

    template<typename TSearch, typename TMap>
    struct MappedType;

    template<typename TSearch, typename TKey, typename T, typename TForwardConverter, typename TBackwardConverter, 
        typename... TKeys, typename... Ts, typename... TForwardConverters, typename... TBackwardConverters>
    struct MappedType<TSearch, TypeMap<TypeAssigment<TypePair<TKey, T>, TForwardConverter, TBackwardConverter>,  
                                       TypeAssigment<TypePair<TKeys, Ts>, TForwardConverters, TBackwardConverters>...>>
    {
        using Type = std::conditional_t<std::is_same_v<TKey, TSearch>,
            T,
            typename MappedType<TSearch, TypeMap<TypeAssigment<TypePair<TKeys, Ts>, TForwardConverters, TBackwardConverters>...>>::Type>;

        using ForwardConverterType = std::conditional_t<std::is_same_v<TKey, TSearch>,
                TForwardConverter,
                typename MappedType<TSearch, TypeMap<TypeAssigment<TypePair<TKeys, Ts>, TForwardConverters, TBackwardConverters>...>>::ForwardConverterType>;
        
        using BackwardConverterType = std::conditional_t<std::is_same_v<TKey, TSearch>,
                TBackwardConverter,
                typename MappedType<TSearch, TypeMap<TypeAssigment<TypePair<TKeys, Ts>, TForwardConverters, TBackwardConverters>...>>::BackwardConverterType>;
    };

    template<typename TSearch>
    struct MappedType<TSearch, void>
    {
        using Type = TSearch;
        using ForwardConverterType = void;
        using BackwardConverterType = void;
    };

    template<typename TSearch>
    struct MappedType<TSearch, TypeMap<>>: MappedType<TSearch, void>
    {
    };

    template<typename TSearch, typename TMap>
    using MappedTypeT =  typename MappedType<TSearch, TMap>::Type;
}