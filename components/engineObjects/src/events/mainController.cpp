#include <dory/engineObjects/events/mainController.h>

namespace dory
{
    template class events::EventController<DataContextType, engine::events::mainController::Initialize>;
    template class events::EventController<DataContextType, engine::events::mainController::Stop>;
}