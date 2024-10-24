#pragma once

#include <functional>
#include <map>
#include <string>
#include <any>
#include <dory/core/resources/dataContext.h>
#include <dory/generic/extension/libraryHandle.h>

namespace dory::core::services
{
    class IScriptService
    {
    public:
        using ScriptParametersPackType = std::map<std::string, std::any>;
        using ScriptFunctionType = std::function<void(resources::DataContext&, const ScriptParametersPackType&)>;

        virtual ~IScriptService() = default;

        virtual void addScript(const std::string& scriptKey, generic::extension::LibraryHandle libraryHandle, ScriptFunctionType script) = 0;
        virtual void removeScript(const std::string& scriptKey) = 0;
        virtual bool runScript(resources::DataContext& context, const std::string& scriptKey, const ScriptParametersPackType& arguments) = 0;
    };
}
