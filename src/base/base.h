#pragma once

using Byte = std::uint8_t;

#include "compileTimeTypes.h"
#include "configuration/configuration.h"
#include "serviceLocator.h"
#include "configuration/fileSystemBasedConfiguration.h"
#include "domain/services/basicFrameService.h"
#include "domain/services/matrixCalculatorService.h"
#include "domain/services/transformationCalculatorService.h"
#include "domain/services/pipelineService.h"
#include "domain/controllers/viewController.h"
#include "domain/geometry/transformation.h"
#include "domain/geometry/mathFunctions.h"
#include "multithreading/individualProcessThread.h"
#include "domain/events/eventHub.h"
#include "domain/events/systemConsoleEventHub.h"
#include "domain/events/windowEventHub.h"