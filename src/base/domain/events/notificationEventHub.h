#pragma once

#include "event.h"

namespace dory::domain::events::notification
{
    namespace application
    {
        struct Notification
        {
            const std::string message;

            explicit Notification(std::string  message):
                message(std::move(message))
            {}
        };

        struct Problem: public Notification
        {
            explicit Problem(const std::string& message):
                    Notification(message)
            {}
        };

        struct Disaster: public Notification
        {
            explicit Disaster(const std::string& message):
                    Notification(message)
            {}
        };

        template<class TDataContext>
        class NotificationEventHub: Uncopyable
        {
        protected:
            EventDispatcher<TDataContext&, const Notification&> notificationEvent;
            EventDispatcher<TDataContext&, const Problem&> problemEvent;
            EventDispatcher<TDataContext&, const Disaster&> disasterEvent;

        public:
            Event<TDataContext&, const Notification&>& onNotification()
            {
                return notificationEvent;
            }

            Event<TDataContext&, const Problem&>& onProblem()
            {
                return problemEvent;
            }

            Event<TDataContext&, const Disaster&>& onDisaster()
            {
                return disasterEvent;
            }
        };

        template<class TDataContext>
        class NotificationEventDispatcher: public NotificationEventHub<TDataContext>
        {
        public:
            void fire(TDataContext& context, const Notification& eventData)
            {
                this->notificationEvent(context, eventData);
            }

            void fire(TDataContext& context, const Problem& eventData)
            {
                this->problemEvent(context, eventData);
            }

            void fire(TDataContext& context, const Disaster& eventData)
            {
                this->disasterEvent(context, eventData);
            }
        };
    }
}