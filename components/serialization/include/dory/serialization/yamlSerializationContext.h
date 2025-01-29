#pragma once

#include <ryml.hpp>

namespace dory::serialization::yaml
{
    struct YamlContext: TreeStructureContext<ryml::NodeRef>
    {
        YamlContext() = default;

        explicit YamlContext(const ryml::NodeRef& root): TreeStructureContext(root)
        {}
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
