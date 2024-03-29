#pragma once

using Byte = std::uint8_t;

#include "domain/service.h"
#include "typeComponents.h"
#include "domain/services/frameService.h"
#include "domain/services/linearAlgebra.h"
#include "domain/services/pipelineService.h"
#include "domain/geometry/transformation.h"
#include "domain/geometry/mathFunctions.h"
#include "concurrency/individualProcessThread.h"
#include "domain/events/eventHub.h"
#include "domain/events/systemConsoleEventHub.h"
#include "domain/events/windowEventHub.h"