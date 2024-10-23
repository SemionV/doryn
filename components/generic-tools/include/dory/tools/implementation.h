#pragma once

#include "dory/generic/typeList.h"

namespace dory::core::generic::implementation
{
    template<template<typename TParameter, typename TPolicy, typename TState> class... TImplementations>
    struct ImplementationList
    {};

    template<typename TImplementationPolicy, typename THierarchyState>
    struct ImplementationLevel;

    template<typename TImplementationPolicy>
    struct HierarchyTop{};

    template<typename TImplementations, template<typename> class THierarchyTop = HierarchyTop>
    struct ImplementationPolicy
    {
        using ImplementationTypes = TImplementations;

        template<typename TPolicy>
        using HierarchyTopType = THierarchyTop<TPolicy>;
    };

    template<typename TInterfaces, typename TImplementations, typename TParameters, typename TCurrentParameter>
    struct HierarchyState
    {};

    template<typename TImplementationPolicy,
            typename TInterfaces,
            template<typename, typename, typename> class TImplementation,
            template<typename, typename, typename> class... TImplementations,
            typename TParameters,
            typename TCurrentParameter>
    struct ImplementationLevel<TImplementationPolicy, HierarchyState<TInterfaces, ImplementationList<TImplementation, TImplementations...>, TParameters, TCurrentParameter>>:
        public TImplementation<TCurrentParameter, TImplementationPolicy, HierarchyState<TInterfaces, ImplementationList<TImplementations...>, TParameters, TCurrentParameter>>
    {};

    template<typename TImplementationPolicy,
            typename TInterfaces,
            typename TParameter,
            typename... TParameters,
            typename TCurrentParameter>
    struct ImplementationLevel<TImplementationPolicy, HierarchyState<TInterfaces, ImplementationList<>, TypeList<TParameter, TParameters...>, TCurrentParameter>>:
        public ImplementationLevel<TImplementationPolicy, HierarchyState<TInterfaces, typename TImplementationPolicy::ImplementationTypes, TypeList<TParameters...>, TParameter>>
    {};

    template<typename TImplementationPolicy,
            typename TInterface,
            typename... TInterfaces,
            typename TCurrentParameter>
    struct ImplementationLevel<TImplementationPolicy, HierarchyState<TypeList<TInterface, TInterfaces...>, ImplementationList<>, TypeList<>, TCurrentParameter>>:
        public TInterface,
        public ImplementationLevel<TImplementationPolicy, HierarchyState<TypeList<TInterfaces...>, ImplementationList<>, TypeList<>, TCurrentParameter>>
    {};

    template<typename TImplementationPolicy, typename TCurrentParameter>
    struct ImplementationLevel<TImplementationPolicy, HierarchyState<TypeList<>, ImplementationList<>, TypeList<>, TCurrentParameter>>:
        public TImplementationPolicy::template HierarchyTopType<TImplementationPolicy>
    {};

    template<typename TInterfaces, typename TParameters, typename TImplementationPolicy>
    using Implementation = ImplementationLevel<TImplementationPolicy, HierarchyState<TInterfaces, ImplementationList<>, TParameters, void>>;
}