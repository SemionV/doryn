#pragma once

#include <dory/engineObjects/resources/dataContext.h>
#include <dory/engine/events/mainController.h>

namespace dory
{
    extern template class __attribute__((visibility("default"))) events::EventController<DataContextType, engine::events::mainController::Initialize>;
    extern template class __attribute__((visibility("default"))) events::EventController<DataContextType, engine::events::mainController::Stop>;
}