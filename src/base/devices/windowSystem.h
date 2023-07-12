#pragma once

#include "device.h"
#include "window.h"

namespace dory
{
    template<typename TWindowParameters>
    class DORY_API WindowSystem: public Device
    {
        public:
            virtual std::shared_ptr<Window> createWindow(const TWindowParameters& parameters) = 0;
    };
}