#pragma once

#include <utility>

#include "base/dependencies.h"
#include "base/typeComponents.h"
#include "base/module.h"

namespace dory::domain::events
{
    template<class... Ts>
    class Callable
    {
    public:
        std::optional<std::weak_ptr<ILibrary>> libraryOption;

    protected:
        template<typename... Args>
        void invoke(Args&&... args) const
        {
            std::invoke(std::forward<Args>(args)...);
        }

    public:
        virtual ~Callable() = default;

        explicit Callable() = default;

        explicit Callable(std::weak_ptr<ILibrary> library):
            libraryOption(library)
        {}

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

        explicit CallableFunction(std::function<void(Ts...)>&& function, std::weak_ptr<ILibrary> library):
            Callable<Ts...>(library),
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

        CallableMemberFunction(T* instance, void (T::* memberFunction)(Ts...), std::weak_ptr<ILibrary> library):
            Callable<Ts...>(library),
            memberFunctionPair(instance, memberFunction)
        {}

        inline void operator()(Ts... arguments) const final
        {
            this->invoke(memberFunctionPair.second, memberFunctionPair.first, arguments...);
        }
    };

    template<class... Ts>
    class Event
    {
    public:
        using KeyType = std::size_t;
        using HandlerType = IResourceHandle<std::function<void(Ts...)>>;

    private:
        KeyType idCounter {};

    protected:
        std::map<KeyType, std::shared_ptr<Callable<Ts...>>> handlers;
        //std::map<KeyType, std::unique_ptr<HandlerType>> _handlers;

    public:
        template<typename F>
        KeyType attachHandler(F&& function)
        {
            auto functor = std::forward<F>(function);
            return attachFunction(std::move(functor));
        }

        template<typename F>
        KeyType attachHandler(std::weak_ptr<ILibrary> library, F&& function)
        {
            auto functor = std::forward<F>(function);
            return attachFunction(library, std::move(functor));
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

        KeyType attachHandler(std::weak_ptr<ILibrary> library, std::function<void(Ts...)>&& functor)
        {
            return attachFunction(library, std::move(functor));
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

        template<typename T>
        KeyType attachHandler(std::weak_ptr<ILibrary> library, T* instance, void (T::* memberFunction)(Ts...))
        {
            return attachMemberFunction(instance, memberFunction);
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

        KeyType attachFunction(std::weak_ptr<ILibrary> library, std::function<void(Ts...)>&& functor)
        {
            auto callable = std::make_shared<CallableFunction<Ts...>>(std::move(functor), library);
            return attachCallable(callable);
        }

        template<typename T>
        KeyType attachMemberFunction(T* instance, void (T::* memberFunction)(Ts...))
        {
            auto callable = std::make_shared<CallableMemberFunction<T, Ts...>>(instance, memberFunction);
            return attachCallable(callable);
        }

        template<typename T>
        KeyType attachMemberFunction(std::weak_ptr<ILibrary> library, T* instance, void (T::* memberFunction)(Ts...))
        {
            auto callable = std::make_shared<CallableMemberFunction<T, Ts...>>(instance, memberFunction, library);
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
    private:
        using EventKeyType = Event<Ts...>::KeyType;
        using HandlerType = Event<Ts...>::HandlerType;

    public:
        void operator()(Ts... arguments)
        {
            {
                auto expiredHandles = std::vector<EventKeyType>{};

                for (const auto &[key, handler]: this->handlers) {
                    if (!invokeModuleProcedure(handler->libraryOption, [&]() { (*handler)(arguments...); })) {
                        expiredHandles.emplace_back(key);
                    }
                }

                for (auto &key: expiredHandles) {
                    this->handlers.erase(key);
                }
            }

            /*new handlers*/
            /*{
                auto expiredHandles = std::vector<EventKeyType>{};

                for (std::pair<const EventKeyType, std::unique_ptr<HandlerType>>& handlerPair: this->_handlers) {
                    auto resourceRef = handlerPair.second->lock();
                    if(resourceRef)
                    {
                        //resourceRef.operator->()(arguments...);
                    }
                    else
                    {
                        expiredHandles.emplace_back(handlerPair.first);
                    }
                }
            }*/
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
    class EventHub: NonCopyable, public EventController<TDataContext, TEvents>...
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

        template<typename TEvent, typename T>
        auto attach(std::weak_ptr<ILibrary> library, T* instance, void (T::* memberFunction)(TDataContext&, TEvent&))
        {
            return getDispatcher<TEvent>().attachHandler(library, instance, memberFunction);
        }

        template<typename TEvent>
        auto attach(std::function<void(TDataContext&, TEvent&)>&& predicate)
        {
            return getDispatcher<TEvent>().attachHandler(std::move(predicate));
        }

        template<typename TEvent>
        auto attach(std::weak_ptr<ILibrary> library, std::function<void(TDataContext&, TEvent&)>&& predicate)
        {
            return getDispatcher<TEvent>().attachHandler(library, std::move(predicate));
        }
    };

    template<typename TEventHub>
    class EventCannon;

    template<typename TDataContext, typename... TEvents>
    class EventCannon<EventHub<TDataContext, TEvents...>>: public EventHub<TDataContext, TEvents...>
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
                                                                 public EventHub<TDataContext, TEvents...>
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
}