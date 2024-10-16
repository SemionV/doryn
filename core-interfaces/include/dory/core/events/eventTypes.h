#pragma once

#include <any>
#include <dory/core/generic/events.h>
#include <dory/core/resources/dataContext.h>

namespace dory::core::events
{
    namespace mainController
    {
        struct Initialize {};
        struct Stop {};

        using EventList = generic::TypeList<const Initialize, const Stop>;
        using EventHub = dory::core::events::EventHub<resources::DataContext, EventList>;
        using EventDispatcher = dory::core::events::EventCannon<EventHub>;
    }

    extern template class dory::core::events::EventHub<resources::DataContext, mainController::EventList>;
    extern template class dory::core::events::EventCannon<mainController::EventHub>;

    namespace application
    {
        struct Exit {};

        using EventList = generic::TypeList<const Exit>;
        using EventHub = dory::core::events::EventHub<resources::DataContext, EventList>;
        using EventDispatcher = dory::core::events::EventCannon<EventHub>;
    }

    extern template class dory::core::events::EventHub<resources::DataContext, application::EventList>;
    extern template class dory::core::events::EventCannon<application::EventHub>;

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

        using EventList = generic::TypeList<const KeyPressEvent>;
        using EventHub = dory::core::events::EventHub<resources::DataContext, EventList>;
        using EventDispatcher = dory::core::events::EventCannon<EventHub>;
    }

    extern template class dory::core::events::EventHub<resources::DataContext, io::EventList>;
    extern template class dory::core::events::EventCannon<io::EventHub>;

    namespace script
    {
        struct Run
        {
            const std::string scriptKey;
            const std::map<std::string, std::any> arguments;
        };

        using EventList = generic::TypeList<const Run>;
        using EventHub = dory::core::events::EventHub<resources::DataContext, EventList>;
        using EventDispatcher = dory::core::events::EventCannon<EventHub>;
    }

    extern template class dory::core::events::EventHub<resources::DataContext, script::EventList>;
    extern template class dory::core::events::EventCannon<script::EventHub>;

    namespace window
    {
        struct Close
        {
            int windowId;
        };

        using EventList = generic::TypeList<const Close>;
        using EventHub = dory::core::events::EventHub<resources::DataContext, EventList>;
        using EventDispatcher = dory::core::events::EventCannon<EventHub>;
    }

    extern template class dory::core::events::EventHub<resources::DataContext, window::EventList>;
    extern template class dory::core::events::EventCannon<window::EventHub>;
}