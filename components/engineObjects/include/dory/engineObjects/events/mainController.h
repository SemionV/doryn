#pragma once

#include <dory/engine/events/mainController.h>
#include <dory/engineObjects/resources/dataContext.h>

namespace dory
{
    extern template class dory::events::EventHub<DataContextType, engine::events::mainController::EventTypes>;
    extern template class dory::events::EventCannon<dory::events::EventHub<DataContextType, engine::events::mainController::EventTypes>>;
}