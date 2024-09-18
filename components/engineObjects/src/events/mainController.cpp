#include <dory/engineObjects/events/mainController.h>

namespace dory
{
    template class dory::events::EventHub<DataContextType, engine::events::mainController::EventTypes>;
    template class dory::events::EventCannon<dory::events::EventHub<DataContextType, engine::events::mainController::EventTypes>>;
}