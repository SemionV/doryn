#pragma once

using Byte = std::uint8_t;

#include "domain/service.h"
#include "compileTimeTypes.h"
#include "configuration/configuration.h"
#include "serviceLocator.h"
#include "configuration/fileSystemBasedConfiguration.h"
#include "domain/services/frameService.h"
#include "domain/services/matrixCalculatorService.h"
#include "domain/services/transformationCalculatorService.h"
#include "domain/services/pipelineService.h"
#include "domain/geometry/transformation.h"
#include "domain/geometry/mathFunctions.h"
#include "concurrency/individualProcessThread.h"
#include "domain/events/eventHub.h"
#include "domain/events/systemConsoleEventHub.h"
#include "domain/events/windowEventHub.h"