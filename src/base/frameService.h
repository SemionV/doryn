#pragma once

#include "doryExport.h"
#include "domain/engine.h"

namespace dory
{
    class DORY_API FrameService
    {
        public:
            virtual void startLoop(std::shared_ptr<domain::Engine> engine, DataContext& context) = 0;
            virtual void endLoop() = 0;
    };
}