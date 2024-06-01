#pragma once

#include "base/dependencies.h"

namespace dory::typeMap::yaml
{
    struct YamlContext: TreeStructureContext<ryml::NodeRef>
    {
        explicit YamlContext(ryml::NodeRef root): TreeStructureContext<ryml::NodeRef>(root)
        {}
    };

    c4::csubstr toRymlCStr(const std::string& source)
    {
        return {source.data(), source.size()};
    }

    c4::csubstr toRymlCStr(const std::string_view& source)
    {
        return {source.data(), source.size()};
    }

    c4::substr toRymlStr(std::string& source)
    {
        return {source.data(), source.size()};
    }
}
