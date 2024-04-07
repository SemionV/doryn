#pragma once


#include "base/typeComponents.h"

namespace dory::domain::services
{
    template<typename TImplementation, typename TDataContext>
    struct IScriptService
    {
        using ScriptParametersPackType = std::map<std::string, std::any>;

        template<typename F>
        void addScript(const std::string& scriptKey, F&& script)
        {
            toImplementation<TImplementation>(this)->addScriptImpl(scriptKey, std::forward<F>(script));
        }

        bool runScript(TDataContext& context, const std::string& scriptKey, const ScriptParametersPackType& arguments)
        {
            return toImplementation<TImplementation>(this)->runScriptImpl(context, scriptKey, arguments);
        }
    };

    template<typename TDataContext>
    class ScriptService: Uncopyable, public IScriptService<ScriptService<TDataContext>, TDataContext>
    {
    private:
        using ParentScriptParametersPackType = IScriptService<ScriptService<TDataContext>, TDataContext>::ScriptParametersPackType;
        using ScriptFunctionType = void(TDataContext&, const ParentScriptParametersPackType&);

        std::unordered_map<std::string, std::function<ScriptFunctionType>> scripts;

    public:

        template<typename F>
        void addScriptImpl(const std::string& scriptKey, F&& script)
        {
            scripts.emplace(scriptKey, std::forward<F>(script));
        }

        bool runScriptImpl(TDataContext& context, const std::string& scriptKey, const ParentScriptParametersPackType& arguments)
        {
            if(scripts.contains(scriptKey))
            {
                auto script = scripts[scriptKey];
                script(context, arguments);

                return true;
            }

            return false;
        }
    };
}