#pragma once

#include <dory/engine/events/mainController.h>
#include <dory/engineObjects/resources/dataContext.h>

namespace dory
{
    extern template class dory::events::EventHub<DataContextType, const engine::events::mainController::Initialize, const engine::events::mainController::Stop>;
}