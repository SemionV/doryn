#pragma once

#include "base/doryExport.h"
#include "base/domain/engine.h"

namespace dory::domain::services
{
    class DORY_API IFrameService
    {
        public:
            virtual void startLoop(std::shared_ptr<domain::Engine> engine, DataContext& context) = 0;
            virtual void endLoop() = 0;
    };
}