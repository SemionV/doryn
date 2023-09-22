#pragma once

#include "base/doryExport.h"
#include "base/domain/engine.h"

namespace dory::domain::services
{
    template<class TDataContext>
    class IFrameService
    {
        public:
            virtual void startLoop(std::shared_ptr<domain::Engine<TDataContext>> engine, TDataContext& context) = 0;
            virtual void endLoop() = 0;
    };
}