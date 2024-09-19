#include <dory/engineObjects/controllers/mainController.h>

namespace dory::engine::controllers
{
    template class MainController<DataContextType, repositories::PipelineRepositoryType>;
}