#pragma once

#include "dory/core/generic/typeTraits.h"
#include "dory/core/generic/typeList.h"
#include <dory/generic/extension/resourceHandle.h>
#include "implementation.h"
#include <mutex>
#include <functional>

namespace dory::core::generic::events
{
    template<class... Ts>
    class Callable
    {
    protected:
        template<typename... Args>
        void invoke(Args&&... args) const
        {
            std::invoke(std::forward<Args>(args)...);
        }

    public:
        virtual ~Callable() = default;

        explicit Callable() = default;

        virtual void operator()(Ts... arguments) const = 0;


    };

    template<class... Ts>
    class CallableFunction: public Callable<Ts...>
    {
    private:
        std::function<void(Ts...)> function;

    public:
        explicit CallableFunction(std::function<void(Ts...)>&& function):
            function(std::move(function))
        {}

        inline void operator()(Ts... arguments) const final
        {
            this->invoke(function, arguments...);
        }
    };

    template<typename T, class... Ts>
    class CallableMemberFunction: public Callable<Ts...>
    {
    private:
        std::pair<T*, void (T::*)(Ts...)> memberFunctionPair;

    public:
        CallableMemberFunction(T* instance, void (T::* memberFunction)(Ts...)):
            memberFunctionPair(instance, memberFunction)
        {}

        inline void operator()(Ts... arguments) const final
        {
            this->invoke(memberFunctionPair.second, memberFunctionPair.first, arguments...);
        }
    };

    using EventKeyType = std::size_t;

    template<class... Ts>
    class Event
    {
    public:
        using KeyType = EventKeyType;
        using HandlerType = std::function<void(Ts...)>;

    private:
        KeyType idCounter {};

    protected:
        std::map<KeyType, std::shared_ptr<Callable<Ts...>>> handlers;
        //TODO: if the event used concurrently, it can lead to data race
        std::unordered_map<KeyType, std::shared_ptr<dory::generic::extension::ResourceHandle<HandlerType>>> _handlers;

    public:
        template<typename F>
        KeyType attachHandler(F&& function)
        {
            auto functor = std::forward<F>(function);
            return attachFunction(std::move(functor));
        }

        template<typename F>
        KeyType operator+=(F&& function)
        {
            return attachHandler(std::forward<F>(function));
        }

        KeyType attachHandler(std::function<void(Ts...)>&& functor)
        {
            return attachFunction(std::move(functor));
        }

        template<typename F>
        KeyType operator+=(std::function<void(Ts...)>&& functor)
        {
            return attachFunction(std::move(functor));
        }

        template<typename T>
        KeyType attachHandler(T* instance, void (T::* memberFunction)(Ts...))
        {
            return attachMemberFunction(instance, memberFunction);
        }

        KeyType attachHandlerLib(std::shared_ptr<dory::generic::extension::ResourceHandle<HandlerType>> handler)
        {
            KeyType key = getNewKey();
            _handlers[key] = handler;

            return key;
        }

        void detachHandler(KeyType handlerKey)
        {
            handlers.erase(handlerKey);
        }

    private:
        KeyType attachFunction(std::function<void(Ts...)>&& functor)
        {
            auto callable = std::make_shared<CallableFunction<Ts...>>(std::move(functor));
            return attachCallable(callable);
        }

        template<typename T>
        KeyType attachMemberFunction(T* instance, void (T::* memberFunction)(Ts...))
        {
            auto callable = std::make_shared<CallableMemberFunction<T, Ts...>>(instance, memberFunction);
            return attachCallable(callable);
        }

        KeyType attachCallable(std::shared_ptr<Callable<Ts...>> callable)
        {
            KeyType key = getNewKey();
            handlers.emplace(key, callable);

            return key;
        }

        KeyType getNewKey()
        {
            return idCounter++;
        }
    };

    template<class... Ts>
    class EventDispatcher: public Event<Ts...>
    {
    public:
        using EventKeyType = Event<Ts...>::KeyType;
        using HandlerType = Event<Ts...>::HandlerType;

    public:
        void operator()(Ts... arguments)
        {
            {
                auto expiredHandles = std::vector<EventKeyType>{};

                for (const auto &[key, handler]: this->handlers) {
                    (*handler)(arguments...);
                }
            }

            /*new handlers*/
            {
                auto expiredHandles = std::vector<EventKeyType>{};

                for (std::pair<const EventKeyType, std::shared_ptr<dory::generic::extension::ResourceHandle<HandlerType>>>& handlerPair: this->_handlers) {
                    auto resourceRef = handlerPair.second->lock();
                    if(resourceRef) {
                        (*resourceRef)(arguments...);
                    }
                    else {
                        expiredHandles.emplace_back(handlerPair.first);
                    }
                }

                for (auto &key: expiredHandles) {
                    this->_handlers.erase(key);
                }
            }
        }
    };

    template<typename TDataContext, typename TEventData>
    class EventBuffer
    {
    private:
        std::shared_ptr<std::vector<TEventData>> eventCases;
        std::shared_ptr<std::vector<TEventData>> eventCasesBackBuffer;
        std::mutex mutex;

    public:
        EventBuffer():
                eventCases(std::make_shared<std::vector<TEventData>>()),
                eventCasesBackBuffer(std::make_shared<std::vector<TEventData>>())
        {}

        void addCase(const TEventData& eventData)
        {
            std::lock_guard lock(mutex);
            eventCasesBackBuffer->emplace_back(std::move(eventData));
        }

        template<typename TDispatcher>
        void submitCases(TDispatcher& eventDispatcher, TDataContext& dataContext)
        {
            std::lock_guard lock{mutex};

            std::shared_ptr<std::vector<TEventData>> temp = eventCases;
            eventCases = eventCasesBackBuffer;
            eventCasesBackBuffer = temp;

            for(std::size_t i = 0; i < eventCases->size(); ++i)
            {
                try
                {
                    eventDispatcher(dataContext, eventCases->operator[](i));
                }
                catch(const std::exception& e)
                {
                }
            }
            eventCases->clear();
        }
    };

    template<typename TDataContext, typename TEventData>
    class EventController
    {
    protected:
        EventDispatcher<TDataContext&, TEventData&> eventDispatcher;
    };

    template<typename TDataContext, typename... TEvents>
    class EventHub: generic::NonCopyable, public EventController<TDataContext, TEvents>...
    {
    protected:
        template<typename TEvent>
        EventDispatcher<TDataContext&, TEvent&>& getDispatcher()
        {
            return EventController<TDataContext, TEvent>::eventDispatcher;
        }

    public:
        template<typename TEvent, typename T>
        auto attach(T* instance, void (T::* memberFunction)(TDataContext&, TEvent&))
        {
            return getDispatcher<TEvent>().attachHandler(instance, memberFunction);
        }

        template<typename TEvent>
        auto attach(std::function<void(TDataContext&, TEvent&)>&& predicate)
        {
            return getDispatcher<TEvent>().attachHandler(std::move(predicate));
        }

        template<typename TEvent>
        void detach(EventKeyType eventKey)
        {
            getDispatcher<TEvent>().detachHandler(eventKey);
        }
    };

    template<typename TDataContext, typename... TEvents>
    class EventHub<TDataContext, generic::TypeList<TEvents...>>: public EventHub<TDataContext, TEvents...>
    {};

    template<typename TDataContext, typename... TEvents>
    class EventCannon: public EventHub<TDataContext, TEvents...>
    {
    public:
        template<typename TEvent>
        void fire(TDataContext& context, TEvent& eventData)
        {
            auto dispatcher = this->template getDispatcher<TEvent>();
            dispatcher(context, eventData);
        }

        template<typename TEvent>
        void fire(TDataContext& context, const TEvent& eventData)
        {
            auto dispatcher = this->template getDispatcher<const TEvent>();
            dispatcher(context, eventData);
        }
    };

    template<typename TDataContext, typename... TEvents>
    class EventCannon<EventHub<TDataContext, TEvents...>>: public EventCannon<TDataContext, TEvents...>
    {};

    template<typename EventHub>
    using EventCannonHub = EventCannon<EventHub>;

    template<typename TEventHub>
    class EventCannonBuffer;

    template<typename TDataContext, typename TEventData>
    class EventBufferController
    {
    protected:
        EventBuffer<TDataContext, TEventData> eventBuffer;
    };

    template<class TDataContext, typename... TEvents>
    class EventCannonBuffer<EventHub<TDataContext, TEvents...>>: public EventBufferController<TDataContext, TEvents>...,
                                                                 public EventHub<TDataContext, generic::TypeList<TEvents...>>
    {
    public:
        template<typename TEvent>
        void charge(const TEvent& eventData)
        {
            this->EventBufferController<TDataContext, TEvent>::eventBuffer.addCase(eventData);
        }

        void fireAll(TDataContext& context)
        {
            (EventBufferController<TDataContext, TEvents>::eventBuffer.submitCases(this->template getDispatcher<TEvents>(), context), ...);
        }
    };

    template<typename TDataContext, typename... TEvents>
    class EventCannonBuffer<EventHub<TDataContext, generic::TypeList<TEvents...>>>: public EventCannonBuffer<EventHub<TDataContext, TEvents...>>
    {};

    template<typename EventHub>
    using EventCannonBufferHub = EventCannonBuffer<EventHub>;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    template<typename TEvent, typename TPolicy, typename TState>
    class ListenerImpl: public implementation::ImplementationLevel<TPolicy, TState>
    {
    public:
        EventKeyType attach(std::function<void(resources::DataContext&, TEvent&)> handler) override
        {
            return this->_hub.attach(std::move(handler));
        }

        void detach(EventKeyType eventKey, const TEvent& event) override
        {
            this->_hub.template detach<TEvent>(eventKey);
        }
    };

    template<typename TEvent, typename TPolicy, typename TState>
    class DispatcherImpl: public implementation::ImplementationLevel<TPolicy, TState>
    {
    public:
        void fire(resources::DataContext& context, TEvent& eventData) override
        {
            this->_dispatcher.fire(context, eventData);
        }
    };

    template<typename TEvent, typename TPolicy, typename TState>
    class DispatcherBufferImpl: public implementation::ImplementationLevel<TPolicy, TState>
    {
    public:
        void fireAll(resources::DataContext& context) override
        {
            this->_dispatcher.fireAll(context);
        }

        void charge(TEvent eventData) override
        {
            this->_dispatcher.charge(eventData);
        }
    };

    template<typename TPolicy>
    class DispatcherTopImpl
    {
    private:
        using EventHubType = EventHub<resources::DataContext, typename TPolicy::EventTypes>;
        using EventDispatcherType = TPolicy::template CannonType<EventHubType>;

    protected:
        EventDispatcherType _dispatcher;
        EventHubType& _hub = _dispatcher;
    };

    template<typename TEvents, template<typename> class TCannon, template<typename, typename, typename> class... TImplementations>
    struct DispatcherImplPolicy: implementation::ImplementationPolicy<
            implementation::ImplementationList<TImplementations...>,
            DispatcherTopImpl>
    {
        using EventTypes = TEvents;

        template<typename TEventHub>
        using CannonType = TCannon<TEventHub>;
    };

    template<typename TIListener, typename TIDispatcher, typename TEventList>
    using DispatcherCannon = implementation::Implementation<TypeList<TIListener, TIDispatcher>, TEventList, DispatcherImplPolicy<TEventList, EventCannonHub, ListenerImpl, DispatcherImpl>>;

    template<typename TIListener, typename TIDispatcher, typename TEventList>
    using DispatcherCannonBuffer = implementation::Implementation<TypeList<TIListener, TIDispatcher>, TEventList, DispatcherImplPolicy<TEventList, EventCannonBufferHub, ListenerImpl, DispatcherBufferImpl>>;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    template<typename TEvent>
    class IEventListener
    {
    public:
        virtual std::size_t attach(std::function<void(resources::DataContext&, TEvent&)> handler) = 0;
        virtual void detach(std::size_t eventKey, const TEvent& event) = 0;
    };

    template<typename... TEvents>
    class IEventsListener: public IEventListener<TEvents>...
    {
    public:
        virtual ~IEventsListener() = default;

        using IEventListener<TEvents>::attach...;
        using IEventListener<TEvents>::detach...;
    };

    template<typename... TEvents>
    class IEventsListener<generic::TypeList<TEvents...>>: public IEventsListener<TEvents...>
    {};

    template<typename TEvent>
    class IEventDispatcher
    {
    public:
        virtual void fire(resources::DataContext& context, TEvent& eventData) = 0;
    };

    template<typename... TEvents>
    class IEventsDispatcher: public IEventDispatcher<TEvents>...
    {
    public:
        virtual ~IEventsDispatcher() = default;

        using IEventDispatcher<TEvents>::fire...;
    };

    template<typename... TEvents>
    class IEventsDispatcher<generic::TypeList<TEvents...>>: public IEventsDispatcher<TEvents...>
    {};

    template<typename TEvent>
    class IEventBufferDispatcher
    {
    public:
        virtual void charge(TEvent eventData) = 0;
        virtual void fireAll(resources::DataContext& context) = 0;
    };

    template<typename... TEvents>
    class IEventsBufferDispatcher: public IEventBufferDispatcher<TEvents>...
    {
    public:
        virtual ~IEventsBufferDispatcher() = default;

        using IEventBufferDispatcher<TEvents>::charge...;
        using IEventBufferDispatcher<TEvents>::fireAll...;
    };

    template<typename... TEvents>
    class IEventsBufferDispatcher<generic::TypeList<TEvents...>>: public IEventsBufferDispatcher<TEvents...>
    {};

    template<typename... TEvents>
    struct EventBundle
    {
        using EventListType = generic::TypeList<TEvents...>;
        using IDispatcher = IEventsDispatcher<EventListType>;
        using IListener = IEventsListener<EventListType>;
    };

    template<typename... TEvents>
    struct EventBufferBundle
    {
        using EventListType = generic::TypeList<TEvents...>;
        using IDispatcher = IEventsBufferDispatcher<EventListType>;
        using IListener = IEventsListener<EventListType>;
    };
}