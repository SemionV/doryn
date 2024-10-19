#pragma once

#include <any>
#include <functional>
#include <map>
#include <string>
#include <dory/core/resources/dataContext.h>
#include <dory/core/generic/typeList.h>

namespace dory::core::events
{
    template<typename TEvent>
    class IEventListener
    {
        virtual std::size_t attach(std::function<void(resources::DataContext&, TEvent&)> handler) = 0;
    };

    template<typename... TEvents>
    class IEventsListener: IEventListener<TEvents>...
    {
    public:
        virtual ~IEventsListener() = default;
    };

    template<typename... TEvents>
    class IEventsListener<generic::TypeList<TEvents...>>: IEventsListener<TEvents...>
    {};

    template<typename TEvent>
    class IEventDispatcher
    {
        virtual void fire(resources::DataContext& context, TEvent& eventData) = 0;
    };

    template<typename... TEvents>
    class IEventsDispatcher: IEventDispatcher<TEvents>...
    {
    public:
        virtual ~IEventsDispatcher() = default;
    };

    template<typename... TEvents>
    class IEventsDispatcher<generic::TypeList<TEvents...>>: IEventsDispatcher<TEvents...>
    {};

    namespace mainController
    {
        struct Initialize {};
        struct Stop {};

        using EventListType = generic::TypeList<const Initialize, const Stop>;
        class IDispatcher: public IEventsDispatcher<EventListType> {};
        class IListener: public IEventsListener<EventListType> {};
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

            virtual void fireAll(resources::DataContext& context) = 0;
            virtual void charge(KeyPressEvent eventData) = 0;
        };

        class IEventHub
        {
        public:
            virtual ~IEventHub() = default;

            virtual std::size_t attach(std::function<void(resources::DataContext&, KeyPressEvent&)> handler) = 0;
        };
    }

    namespace script
    {
        struct Run
        {
            const std::string scriptKey;
            const std::map<std::string, std::any> arguments;
        };

        using EventListType = generic::TypeList<const Run>;

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