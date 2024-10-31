#pragma once

#include <dory/core/services/iScriptService.h>
#include <dory/generic/extension/resourceHandle.h>

namespace dory::core
{
    class Registry;
}

namespace dory::core::services
{
    class ScriptService: public IScriptService
    {
    private:
        std::unordered_map<std::string, generic::extension::ResourceHandle<ScriptFunctionType>> _scripts;
        Registry& _registry;

    public:
        explicit ScriptService(Registry& registry);

        void addScript(const std::string& scriptKey, generic::extension::LibraryHandle libraryHandle, ScriptFunctionType script) override;
        void removeScript(const std::string& scriptKey) override;
        bool runScript(resources::DataContext& context, const std::string& scriptKey, const ScriptParametersPackType& arguments) override;
    };
}