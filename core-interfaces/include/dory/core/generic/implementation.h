#pragma once

#include "typeList.h"

namespace dory::core::generic::implementation
{
    template<template<typename TParameter, typename TPolicy, typename TState> class... TImplementations>
    struct ImplementationList
    {};

    template<typename TImplementationPolicy, typename THierarchyState>
    struct ImplementationLevel;

    template<typename TInterfaces, typename TImplementations, typename TParameters, typename TCustomHierarchyTop = void>
    struct ImplementationPolicy
    {
        using InterfaceTypes = TInterfaces;
        using ImplementationTypes = TImplementations;
        using ParameterTypes = TParameters;
        using CustomHierarchyTopType = TCustomHierarchyTop;
    };

    template<typename TImplementations, typename TParameters, typename TCurrentParameter>
    struct HierarchyState
    {};

    template<typename TImplementationPolicy,
            template<typename, typename, typename> class TImplementation,
            template<typename, typename, typename> class... TImplementations,
            typename TParameters,
            typename TCurrentParameter>
    struct ImplementationLevel<TImplementationPolicy, HierarchyState<ImplementationList<TImplementation, TImplementations...>, TParameters, TCurrentParameter>>:
        public TImplementation<TCurrentParameter, TImplementationPolicy, HierarchyState<ImplementationList<TImplementations...>, TParameters, TCurrentParameter>>
    {};

    template<typename TImplementationPolicy,
            typename TParameter,
            typename... TParameters,
            typename TCurrentParameter>
    struct ImplementationLevel<TImplementationPolicy, HierarchyState<ImplementationList<>, TypeList<TParameter, TParameters...>, TCurrentParameter>>:
        public ImplementationLevel<TImplementationPolicy, HierarchyState<typename TImplementationPolicy::ImplementationTypes, TypeList<TParameters...>, TParameter>>
    {};

    template<typename TInterface,
            typename... TInterfaces,
            typename TImplementations,
            typename TParameters,
            typename TCustomHierarchyTop,
            typename TCurrentParameter>
    struct ImplementationLevel<ImplementationPolicy<TypeList<TInterface, TInterfaces...>, TImplementations, TParameters, TCustomHierarchyTop>,
            HierarchyState<ImplementationList<>, TypeList<>, TCurrentParameter>>:
        public TInterface,
        public ImplementationLevel<ImplementationPolicy<TypeList<TInterfaces...>, TImplementations, TParameters, TCustomHierarchyTop>, HierarchyState<ImplementationList<>, TypeList<>, TCurrentParameter>>
    {};

    template<typename TImplementations,
            typename TParameters,
            typename TCustomHierarchyTop,
            typename TCurrentParameter>
    struct ImplementationLevel<ImplementationPolicy<TypeList<>, TImplementations, TParameters, TCustomHierarchyTop>, HierarchyState<ImplementationList<>, TypeList<>, TCurrentParameter>>:
        public TCustomHierarchyTop
    {};

    template<typename TImplementations,
            typename TParameters,
            typename TCurrentParameter>
    struct ImplementationLevel<ImplementationPolicy<TypeList<>, TImplementations, TParameters, void>, HierarchyState<ImplementationList<>, TypeList<>, TCurrentParameter>>
    {};

    template<typename TTImplementationPolicy>
    struct Implementation;

    template<typename TInterfaces, typename TImplementations, typename TParameter, typename... TParameters, typename TCustomHierarchyTop>
    struct Implementation<ImplementationPolicy<TInterfaces, TImplementations, TypeList<TParameter, TParameters...>, TCustomHierarchyTop>>:
            ImplementationLevel<ImplementationPolicy<TInterfaces, TImplementations, TypeList<TParameter, TParameters...>, TCustomHierarchyTop>,
                HierarchyState<TImplementations, TypeList<TParameters...>, TParameter>>
    {};
}