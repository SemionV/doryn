#pragma once

#include "base/dependencies.h"

namespace dory::serialization::json
{
    using json = nlohmann::json;

    struct JsonContext: TreeStructureContext<json*>
    {
        explicit JsonContext(json* data): TreeStructureContext<json *>(data)
        {}
    };
}