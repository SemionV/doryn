#pragma once

#include "base/dependencies.h"

namespace dory::typeMap::json
{
    using json = nlohmann::json;

    struct JsonContext
    {
        std::stack<json*> current;
        std::stack<std::size_t> collectionIndexesStack;
        std::stack<std::queue<std::string>> dictionaryKeysStack;

        explicit JsonContext(json* data)
        {
            current.push(data);
        }
    };
}