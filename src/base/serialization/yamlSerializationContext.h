#pragma once

#include "base/dependencies.h"

namespace dory::typeMap::yaml
{
    struct YamlContext
    {
        std::stack<ryml::NodeRef> current;
        std::stack<std::size_t> collectionIndexesStack;
        std::stack<std::queue<std::string>> dictionaryKeysStack;

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
