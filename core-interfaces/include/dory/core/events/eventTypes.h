#pragma once

#include <any>
#include <functional>
#include <map>
#include <string>
#include <dory/core/resources/dataContext.h>
#include <dory/core/generic/typeList.h>
#include <dory/core/generic/events.h>

namespace dory::core::events
{
    struct mainController
    {
        struct Initialize {};
        struct Stop {};

        using Bundle = EventBundle<const Initialize, const Stop>;
    };

    namespace application
    {
        struct Exit {};

        using Bundle = EventBundle<const Exit>;
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

        using Bundle = EventBufferBundle<KeyPressEvent>;
    }

    namespace script
    {
        struct Run
        {
            const std::string scriptKey;
            const std::map<std::string, std::any> arguments;
        };

        using Bundle = EventBundle<const Run>;
    }

    namespace window
    {
        struct Close
        {
            int windowId;
        };

        using Bundle = EventBundle<const Close>;;
    }
}