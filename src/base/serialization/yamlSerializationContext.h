#pragma once

#include "base/dependencies.h"

namespace dory::typeMap::yaml
{
    struct YamlContext
    {
        std::stack<ryml::NodeRef> current;
        std::size_t dynamicCollectionIndex = 0;
        std::size_t previousDynamicCollectionIndex = 0;

        explicit YamlContext(ryml::NodeRef root)
        {
            current.push(root);
        }
    };
}
