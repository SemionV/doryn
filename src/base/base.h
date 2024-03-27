#pragma once

using Byte = std::uint8_t;

class Uncopyable
{
public:
    Uncopyable(const Uncopyable&) = delete;
    Uncopyable& operator=(const Uncopyable&) = delete;

    Uncopyable() = default;
};

#include "domain/service.h"
#include "typeComponents.h"
#include "serviceLocator.h"
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