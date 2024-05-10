#pragma once

#include "base/dependencies.h"

namespace dory::typeMap::yaml
{
    struct YamlContext
    {
        std::stack<ryml::NodeRef> current;
        std::size_t dynamicCollectionIndex = 0;
        std::size_t previousDynamicCollectionIndex = 0;
        std::vector<std::shared_ptr<std::string>> strings;

        explicit YamlContext(ryml::NodeRef root)
        {
            current.push(root);
        }
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
