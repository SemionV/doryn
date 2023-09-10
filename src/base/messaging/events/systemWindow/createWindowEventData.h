#pragma once

#include "base/dependencies.h"
#include "base/domain/window.h"

namespace dory
{
    struct CreateWindowEventData
    {
        public:
            const bool isSuccessfull;
            const std::shared_ptr<Window> window;

        public:
            CreateWindowEventData(std::shared_ptr<Window> window, bool isSuccessfull):
                window(window),
                isSuccessfull(isSuccessfull)
            {                
            }
    };
}