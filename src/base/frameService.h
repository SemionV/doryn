#pragma once

#include "doryExport.h"
#include "domain/engine.h"

namespace dory
{
    class DORY_API FrameService
    {
        public:
            FrameService();
            virtual ~FrameService();

            virtual void startLoop(Engine& engine) = 0;
            virtual void endLoop() = 0;
    };
}