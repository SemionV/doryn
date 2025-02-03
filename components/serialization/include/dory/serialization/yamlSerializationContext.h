#pragma once

#include <ryml.hpp>

namespace dory::serialization::yaml
{
    template<typename TRegistry, typename TDataContext>
    struct YamlContext: TreeStructureContext<ryml::NodeRef, TRegistry, TDataContext>
    {
        explicit YamlContext(const ryml::NodeRef& root, TRegistry& registry, TDataContext& dataContext):
            TreeStructureContext<ryml::NodeRef, TRegistry, TDataContext>(root, registry, dataContext)
        {}

        YamlContext(const YamlContext& other) = default;
    };

    template<typename T>
    c4::csubstr toRymlCStr(const T& source)
    {
        return {source.data(), source.size()};
    }

    template<typename T>
    c4::substr toRymlStr(T& source)
    {
        return {source.data(), source.size()};
    }
}
