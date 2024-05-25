#pragma once

#include "event.h"

namespace dory::domain::events
{
    namespace script
    {
        struct RunScriptEventData
        {
            const std::string scriptKey;
            const std::map<std::string, std::any> arguments;
        };
    }

    template<class TDataContext>
    class ScriptEventHub: Uncopyable
    {
    protected:
        EventDispatcher<TDataContext&, const script::RunScriptEventData&> runScriptEvent;

    public:
        Event<TDataContext&, const script::RunScriptEventData&>& onRunScript()
        {
            return runScriptEvent;
        }
    };

    template<class TDataContext>
    class ScriptEventDispatcher: public ScriptEventHub<TDataContext>
    {
    public:
        void fire(TDataContext& context, const script::RunScriptEventData& eventData)
        {
            auto dispatcher = this->runScriptEvent;
            dispatcher(context, eventData);
        }
    };
}
