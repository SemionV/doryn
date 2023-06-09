#pragma once

#include "device.h"
#include "window.h"

namespace dory
{
    template<typename TWindowParameters>
    class DORY_API WindowSystem: public Device
    {
        protected:
            std::vector<std::shared_ptr<Window>> windows;

        public:
            virtual std::shared_ptr<Window> createWindow(const TWindowParameters& parameters) = 0;
        
        protected:
            virtual void registerWindow(std::shared_ptr<Window> window)
            {
                windows.push_back(window);
            }
    };
}