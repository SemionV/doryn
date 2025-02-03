#pragma once

#include <nlohmann/json.hpp>

namespace dory::serialization::json
{
    using json = nlohmann::json;

    template<typename TRegistry, typename TDataContext>
    struct JsonContext: TreeStructureContext<json*, TRegistry, TDataContext>
    {
        explicit JsonContext(json* data, TRegistry& registry, TDataContext& dataContext):
            TreeStructureContext<json*, TRegistry, TDataContext>(data, registry, dataContext)
        {}
    };
}