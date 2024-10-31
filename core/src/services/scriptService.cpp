#include <dory/core/services/scriptService.h>
#include <dory/core/registry.h>

namespace dory::core::services
{
    ScriptService::ScriptService(Registry &registry):
        _registry(registry)
    {}

    void ScriptService::addScript(const std::string& scriptKey, generic::extension::LibraryHandle libraryHandle, ScriptFunctionType script)
    {
        _scripts[scriptKey] = generic::extension::ResourceHandle<ScriptFunctionType>{libraryHandle, script};
    }

    void ScriptService::removeScript(const std::string& scriptKey)
    {
        _scripts.erase(scriptKey);
    }

    bool ScriptService::runScript(resources::DataContext& context, const std::string& scriptKey, const IScriptService::ScriptParametersPackType& arguments)
    {
        if(_scripts.contains(scriptKey))
        {
            auto handle = _scripts[scriptKey];
            auto resourceRef = handle.lock();
            if(resourceRef)
            {
                auto scriptEventDispatcher = _registry.get<events::script::Bundle::IDispatcher>();
                if(scriptEventDispatcher)
                {
                    scriptEventDispatcher->fire(context, events::script::Run{scriptKey});
                }

                (*resourceRef)(context, arguments);
            }
            else
            {
                _scripts.erase(scriptKey);
            }
        }

        return false;
    }
}