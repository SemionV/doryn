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

        class IEventDispatcher
        {
        public:
            virtual ~IEventDispatcher() = default;

            virtual void fire(resources::DataContext& context, const Initialize& eventData) = 0;
            virtual void fire(resources::DataContext& context, const Stop& eventData) = 0;
        };

        class IEventHub
        {
        public:
            virtual ~IEventHub() = default;

            virtual std::size_t attach(std::function<void(resources::DataContext&, const Initialize&)> handler) = 0;
            virtual std::size_t attach(std::function<void(resources::DataContext&, const Stop&)> handler) = 0;
        };
    }

    namespace application
    {
        struct Exit {};

        class IEventDispatcher
        {
        public:
            virtual ~IEventDispatcher() = default;

            virtual void fire(resources::DataContext& context, const Exit& eventData) = 0;
        };

        class IEventHub
        {
        public:
            virtual ~IEventHub() = default;

            virtual std::size_t attach(std::function<void(resources::DataContext&, const Exit&)> handler) = 0;
        };
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

        class IEventDispatcher
        {
        public:
            virtual ~IEventDispatcher() = default;

            virtual void fire(resources::DataContext& context, const KeyCode& eventData) = 0;
        };

        class IEventHub
        {
        public:
            virtual ~IEventHub() = default;

            virtual std::size_t attach(std::function<void(resources::DataContext&, const KeyCode&)> handler) = 0;
        };
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

        class IEventDispatcher
        {
        public:
            virtual ~IEventDispatcher() = default;

            virtual void fire(resources::DataContext& context, const Close& eventData) = 0;
        };

        class IEventHub
        {
        public:
            virtual ~IEventHub() = default;

            virtual std::size_t attach(std::function<void(resources::DataContext&, const Close&)> handler) = 0;
        };
    }
}