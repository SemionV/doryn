#pragma once

#include "doryExport.h"
#include "domain/engine.h"

namespace dory
{
    class DORY_API FrameService
    {
        public:
            virtual void startLoop(std::shared_ptr<Engine> engine) = 0;
            virtual void endLoop() = 0;
    };
}