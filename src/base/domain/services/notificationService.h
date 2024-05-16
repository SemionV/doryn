#pragma once

#include "base/dependencies.h"
#include "base/typeComponents.h"
#include "base/domain/events/notificationEventHub.h"

namespace dory::domain::services
{
    template<typename TImplementation>
    class INotificationService: Uncopyable, public StaticInterface<TImplementation>
    {
    public:
        void sendNotification(TImplementation::DataContextType& context, const std::string& notification)
        {
            this->toImplementation()->sendNotificationImpl(context, notification);
        }

        void sendProblem(TImplementation::DataContextType& context, const std::string& problem)
        {
            this->toImplementation()->sendProblemImpl(context, problem);
        }

        void sendDisaster(TImplementation::DataContextType& context, const std::string& disaster)
        {
            this->toImplementation()->sendDisasterImpl(context, disaster);
        }
    };

    template<typename TDataContext>
    class NotificationService: public INotificationService<NotificationService<TDataContext>>
    {
    private:
        events::notification::application::NotificationEventDispatcher<TDataContext>& notificationsEventHub;

    public:
        using DataContextType = TDataContext;

        void sendNotification(TDataContext& context, const std::string& notification)
        {
            notificationsEventHub.fire(context, events::notification::application::Notification{notification});
        }

        void sendProblemImpl(TDataContext& context, const std::string& problem)
        {
            notificationsEventHub.fire(context, events::notification::application::Problem{problem});
        }

        void sendDisasterImpl(TDataContext& context, const std::string& disaster)
        {
            notificationsEventHub.fire(context, events::notification::application::Disaster{disaster});
        }
    };
}