#pragma once

namespace dory::domain::services
{
    template<typename TImplementation, typename TDataContext>
    struct IScriptService
    {
        using ScriptParametersPackType = std::map<std::string, std::any>;
        using ScriptFunctionType = std::function<void(TDataContext&, const ScriptParametersPackType&)>;

        template<typename F>
        void addScript(const std::string& scriptKey, F&& script)
        {
            toImplementation<TImplementation>(this)->addScriptImpl(scriptKey, std::forward<F>(script));
        }

        void addScript(const std::string& scriptKey, LibraryHandle libraryHandle, ScriptFunctionType* script)
        {
            toImplementation<TImplementation>(this)->addScriptImpl(scriptKey, libraryHandle, script);
        }

        bool runScript(TDataContext& context, const std::string& scriptKey, const ScriptParametersPackType& arguments)
        {
            return toImplementation<TImplementation>(this)->runScriptImpl(context, scriptKey, arguments);
        }
    };

    template<typename TDataContext>
    class ScriptService: NonCopyable, public IScriptService<ScriptService<TDataContext>, TDataContext>
    {
    private:
        using ParentScriptParametersPackType = IScriptService<ScriptService<TDataContext>, TDataContext>::ScriptParametersPackType;
        using ScriptFunctionType = IScriptService<ScriptService<TDataContext>, TDataContext>::ScriptFunctionType;

        std::unordered_map<std::string, std::shared_ptr<IResourceHandle<ScriptFunctionType>>> _scripts;

    public:

        template<typename F>
        void addScriptImpl(const std::string& scriptKey, F&& script)
        {
            auto handler = makeResourceHandle<ScriptFunctionType>(script);
            if(handler)
            {
                _scripts.emplace(scriptKey, handler);
            }
        }

        void addScriptImpl(const std::string& scriptKey, LibraryHandle libraryHandle, ScriptFunctionType* script)
        {
            auto handler = makeResourceHandle<ScriptFunctionType>(libraryHandle, script);
            if(handler)
            {
                _scripts.emplace(scriptKey, handler);
            }
        }

        bool runScriptImpl(TDataContext& context, const std::string& scriptKey, const ParentScriptParametersPackType& arguments)
        {
            if(_scripts.contains(scriptKey))
            {
                auto handle = _scripts[scriptKey];
                auto resourceRef = handle->lock();
                if(resourceRef) {
                    (*resourceRef)(context, arguments);
                }
                else {
                    _scripts.erase(scriptKey);
                }
            }

            return false;
        }
    };
}