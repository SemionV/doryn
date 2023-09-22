#pragma once

#include "frameService.h"

namespace dory::domain::services
{
    class DORY_API BasicFrameService: public IFrameService
    {
        private:
            bool isStop;

        public:
            void startLoop(std::shared_ptr<domain::Engine> engine, DataContext& context);            
            void endLoop();
    };
}