#pragma once

#include "base/dependencies.h"

namespace dory::typeMap::json
{
    using json = nlohmann::json;

    struct JsonContext
    {
        std::stack<json*> current;
        json emptyJson = {};
        std::size_t dynamicCollectionIndex = 0;
        std::size_t previousDynamicCollectionIndex = 0;

        explicit JsonContext(json* data)
        {
            current.push(data);
        }
    };
}