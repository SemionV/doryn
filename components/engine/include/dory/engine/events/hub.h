#pragma once

#include <any>

#include "event.h"

namespace dory::domain::events
{
    namespace engine
    {
        struct Initialize
        {
        };

        struct Stop
        {
        };

        template<typename TDataContext>
        using Hub = EventHub<TDataContext, const Initialize, const Stop>;

        template<typename TDataContext>
        using Dispatcher = EventCannon<Hub<TDataContext>>;
    }

    namespace application
    {
        struct Exit
        {
        };

        template<typename TDataContext>
        using Hub = EventHub<TDataContext, const Exit>;

        template<typename TDataContext>
        using Dispatcher = EventCannon<Hub<TDataContext>>;
    }

    namespace io
    {
        enum class KeyCode
        {
            Unknown,
            Escape,
            Return,
            Backspace,
            Terminate,
            Character
        };

        struct KeyPressEvent
        {
            KeyCode keyCode = KeyCode::Unknown;
            int character = 0;
        };

        template<typename TDataContext>
        using Hub = EventHub<TDataContext, KeyPressEvent>;

        template<typename TDataContext>
        using Dispatcher = EventCannonBuffer<Hub<TDataContext>>;
    }

    namespace script
    {
        struct Run
        {
            const std::string scriptKey;
            const std::map<std::string, std::any> arguments;
        };

        template<typename TDataContext>
        using Hub = EventHub<TDataContext, const Run>;

        template<typename TDataContext>
        using Dispatcher = EventCannon<Hub<TDataContext>>;
    }

    namespace window
    {
        struct Close
        {
            const int windowId;

            explicit Close(int windowId): windowId(windowId)
            {}
        };

        template<typename TDataContext>
        using Hub = EventHub<TDataContext, Close>;

        template<typename TDataContext>
        using Dispatcher = EventCannonBuffer<Hub<TDataContext>>;
    }
}