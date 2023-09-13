#pragma once

#include "device.h"
#include "base/domain/window.h"

namespace dory
{
    template<typename TWindowParameters, class TWindow>
    class DORY_API WindowSystem: public IDevice
    {
        protected:
            std::vector<std::shared_ptr<TWindow>> windows;

        public:
            virtual std::shared_ptr<TWindow> createWindow(const TWindowParameters& parameters) = 0;
            virtual void closeWindow(std::shared_ptr<TWindow> window) = 0;
        
        protected:
            void registerWindow(std::shared_ptr<TWindow> window)
            {
                windows.push_back(window);
            }

            void unregisterWindow(std::shared_ptr<TWindow> window)
            {
                auto it = windows.begin();
                auto end = windows.end();

                for(; it != end; ++it)
                {
                    if((*it) == window)
                    {
                        windows.erase(it);
                        break;
                    }
                }
            }
    };
}