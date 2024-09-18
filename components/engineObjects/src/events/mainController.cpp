#include <dory/engineObjects/events/mainController.h>

namespace dory
{
    template class dory::events::EventHub<DataContextType, const engine::events::mainController::Initialize, const engine::events::mainController::Stop>;
}