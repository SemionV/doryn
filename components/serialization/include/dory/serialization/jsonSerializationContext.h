#pragma once

#include <nlohmann/json.hpp>

namespace dory::serialization::json
{
    using json = nlohmann::json;

    template<typename TContextPolicies>
    struct JsonContext: generic::serialization::TreeStructureContext<json*, TContextPolicies>
    {
        explicit JsonContext(json* data, const generic::serialization::Context<TContextPolicies>& otherContext):
            generic::serialization::TreeStructureContext<json*, TContextPolicies>(data, otherContext)
        {}

        explicit JsonContext(json* data, generic::serialization::Context<TContextPolicies>&& otherContext):
            generic::serialization::TreeStructureContext<json*, TContextPolicies>(data, otherContext)
        {}
    };
}