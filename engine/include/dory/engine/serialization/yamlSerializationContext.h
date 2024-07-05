#pragma once

namespace dory::serialization::yaml
{
    struct YamlContext: TreeStructureContext<ryml::NodeRef>
    {
        explicit YamlContext(ryml::NodeRef root): TreeStructureContext<ryml::NodeRef>(root)
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
