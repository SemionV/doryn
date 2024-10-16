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

    namespace application
    {
        struct Exit {};

        using EventList = generic::TypeList<const Exit>;
        using EventHub = dory::core::events::EventHub<resources::DataContext, EventList>;
        using EventDispatcher = dory::core::events::EventCannon<EventHub>;
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

        using EventList = generic::TypeList<const KeyPressEvent>;
        using EventHub = dory::core::events::EventHub<resources::DataContext, EventList>;
        using EventDispatcher = dory::core::events::EventCannon<EventHub>;
    }

    namespace script
    {
        struct Run
        {
            const std::string scriptKey;
            const std::map<std::string, std::any> arguments;
        };

        class IEventDispatcher
        {
        public:
            virtual ~IEventDispatcher() = default;

            virtual void fire(resources::DataContext& context, const Run& eventData) = 0;
        };

        class IEventHub
        {
        public:
            virtual ~IEventHub() = default;

            virtual std::size_t attach(std::function<void(resources::DataContext&, const Run&)> handler) = 0;
        };
    }

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
}