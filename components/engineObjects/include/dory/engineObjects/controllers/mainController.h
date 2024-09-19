#pragma once

#include "dory/engineObjects/dataContext.h"
#include <dory/engineObjects/repositories/pipelineRepository.h>
#include <dory/engine/controllers/mainController.h>

namespace dory::engine::controllers
{
    extern template class MainController<DataContextType, repositories::PipelineRepositoryType>;
}