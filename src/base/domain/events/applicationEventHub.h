#pragma once

#include "base/typeComponents.h"
#include "base/domain/events/eventBuffer.h"

namespace dory::domain::events
{
    struct ApplicationExitEventData
    {
    };

    template<class TDataContext>
    class ApplicationEventHub: Uncopyable
    {
    private:
        EventDispatcher<TDataContext&, const ApplicationExitEventData&> exitEvent;

    protected:
        EventDispatcher<TDataContext&, const ApplicationExitEventData&>& onExitDispatcher()
        {
            return exitEvent;
        }

    public:
        Event<TDataContext&, const ApplicationExitEventData&>& onExit()
        {
            return exitEvent;
        }
    };

    template<class TDataContext>
    class ApplicationEventHubDispatcher: public ApplicationEventHub<TDataContext>
    {
    public:
        void fire(TDataContext& context, const ApplicationExitEventData& exitEventData)
        {
            auto dispatcher = this->onExitDispatcher();
            dispatcher(context, exitEventData);
        }
    };
}
