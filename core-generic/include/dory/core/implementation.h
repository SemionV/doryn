#pragma once

#include "dory/generic/typeList.h"
#include <utility>

/*

ImplementationLevel<
    TImplementationPolicy,
    HierarchyState<TInterfaces,
    ImplementationList<
        TImplementation, TImplementations...>, TParameters, TCurrentParameter>>
|
TImplementationPolicy,
HierarchyState<TInterfaces = (Interface1, Interface2),
typename TImplementationPolicy::ImplementationTypes = (Implementation1, Implementation2),
generic::TypeList<TParameters...> = [Param2, Param3],
TParameter = Param1>
|
ImplementationLevel<
    TImplementationPolicy,
    HierarchyState<
        TInterfaces = (Interface1, Interface2),
        ImplementationList<>,
        generic::TypeList<
            TParameter = Param1,
            TParameters... = [Param2, Param3]
        >,
        TCurrentParameter = void
    >
>

|
Implementation1
|
|
TImplementationPolicy<
    ImplementationTypes = (Implementation1, Implementation2),
    HierarchyTopType = Class1
>,
HierarchyState<
    TInterfaces = (Interface1, Interface2),
    TImplementations = Implementation1, (Implementation2),
    TParameters = (Param2, Param3),
    TCurrentParameter = Param1
>
|
ImplementationLevel ---------------------
|
|
TImplementationPolicy<
    ImplementationTypes = (Implementation1, Implementation2),
    HierarchyTopType = Class1
>,
HierarchyState<
    TInterfaces = (Interface1, Interface2),
    TImplementations = Implementation1, (Implementation2),
    TParameters = (Param2, Param3),
    TCurrentParameter = Param1
>
|
ImplementationLevel ---------------------
|
|
TImplementationPolicy<
    ImplementationTypes = (Implementation1, Implementation2),
    HierarchyTopType = Class1
>,
HierarchyState<
    TInterfaces = (Interface1, Interface2),
    TImplementations = (),
    TParameters = (Param1, Param2, Param3),
    TCurrentParameter = void
>
|

 */

namespace dory::core::implementation
{
    /*
     * List of Interface-unit implementations
     */
    template<template<typename TParameter, typename TPolicy, typename TState> class... TImplementations>
    struct ImplementationList
    {};

    /*
     * Parent class of an implementation of an Interface-unit
     */
    template<typename TImplementationPolicy, typename THierarchyState>
    struct ImplementationLevel;

    /*
     * Default top class of the Hierarchy
     */
    template<typename TImplementationPolicy>
    struct HierarchyTop
    {
        template<typename... Ts>
        explicit HierarchyTop(Ts&&... parameters)
        {}
    };

    /*
     * Utility Container for the whole Hierarchy
     */
    template<typename TImplementations, template<typename> class THierarchyTop = HierarchyTop>
    struct ImplementationPolicy
    {
        using ImplementationTypes = TImplementations;

        template<typename TPolicy>
        using HierarchyTopType = THierarchyTop<TPolicy>;
    };

    /*
     * Descriptor of a level of Hierarchy
     */
    template<typename TInterfaces, typename TImplementations, typename TParameters, typename TCurrentParameter>
    struct HierarchyState
    {};

    /*
     * Middle level of Hierarchy
     */
    template<typename TImplementationPolicy,
            typename TInterfaces,
            template<typename, typename, typename> class TImplementation,
            template<typename, typename, typename> class... TImplementations,
            typename TParameters,
            typename TCurrentParameter>
    struct ImplementationLevel<TImplementationPolicy, HierarchyState<TInterfaces, ImplementationList<TImplementation, TImplementations...>, TParameters, TCurrentParameter>>:
        public TImplementation<TCurrentParameter, TImplementationPolicy, HierarchyState<TInterfaces, ImplementationList<TImplementations...>, TParameters, TCurrentParameter>>
    {
        template<typename... Ts>
        explicit ImplementationLevel(Ts&&... parameters):
                TImplementation<TCurrentParameter, TImplementationPolicy, HierarchyState<TInterfaces, ImplementationList<TImplementations...>, TParameters, TCurrentParameter>>(std::forward<Ts>(parameters)...)
        {}
    };

    /*
     * Lowest level of Hierarchy
     */
    template<typename TImplementationPolicy,
            typename TInterfaces,
            typename TParameter,
            typename... TParameters,
            typename TCurrentParameter>
    struct ImplementationLevel<TImplementationPolicy, HierarchyState<TInterfaces, ImplementationList<>, generic::TypeList<TParameter, TParameters...>, TCurrentParameter>>:
        public ImplementationLevel<TImplementationPolicy, HierarchyState<TInterfaces, typename TImplementationPolicy::ImplementationTypes, generic::TypeList<TParameters...>, TParameter>>
    {
        template<typename... Ts>
        explicit ImplementationLevel(Ts&&... parameters):
                ImplementationLevel<TImplementationPolicy, HierarchyState<TInterfaces, typename TImplementationPolicy::ImplementationTypes, generic::TypeList<TParameters...>, TParameter>>(std::forward<Ts>(parameters)...)
        {}
    };

    template<typename TImplementationPolicy,
            typename TInterface,
            typename... TInterfaces,
            typename TCurrentParameter>
    struct ImplementationLevel<TImplementationPolicy, HierarchyState<generic::TypeList<TInterface, TInterfaces...>, ImplementationList<>, generic::TypeList<>, TCurrentParameter>>:
        public TInterface,
        public ImplementationLevel<TImplementationPolicy, HierarchyState<generic::TypeList<TInterfaces...>, ImplementationList<>, generic::TypeList<>, TCurrentParameter>>
    {
        template<typename... Ts>
        explicit ImplementationLevel(Ts&&... parameters):
                ImplementationLevel<TImplementationPolicy, HierarchyState<generic::TypeList<TInterfaces...>, ImplementationList<>, generic::TypeList<>, TCurrentParameter>>(std::forward<Ts>(parameters)...)
        {}
    };

    template<typename TImplementationPolicy, typename TCurrentParameter>
    struct ImplementationLevel<TImplementationPolicy, HierarchyState<generic::TypeList<>, ImplementationList<>, generic::TypeList<>, TCurrentParameter>>:
        public TImplementationPolicy::template HierarchyTopType<TImplementationPolicy>
    {
        template<typename... Ts>
        explicit ImplementationLevel(Ts&&... parameters):
                TImplementationPolicy::template HierarchyTopType<TImplementationPolicy>(std::forward<Ts>(parameters)...)
        {}
    };

    template<typename TInterfaces, typename TParameters, typename TImplementationPolicy>
    using Implementation = ImplementationLevel<TImplementationPolicy, HierarchyState<TInterfaces, ImplementationList<>, TParameters, void>>;
}