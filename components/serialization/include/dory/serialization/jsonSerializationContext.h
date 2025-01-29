#pragma once

#include <nlohmann/json.hpp>

namespace dory::serialization::json
{
    using json = nlohmann::json;

    struct JsonContext: TreeStructureContext<json*>
    {
        explicit JsonContext(json* data): TreeStructureContext(data)
        {}
    };
}