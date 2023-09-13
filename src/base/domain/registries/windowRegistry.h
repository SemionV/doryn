#pragma once

#include "base/dependencies.h"

namespace dory
{
    template<class TWindow, class TWindowParameters>
    class WindowRegistry
    {
        private:
            std::vector<TWindow> windows;

        public:
            TWindow& createWindow(const TWindowParameters& windowParameters)
            {
                TWindow& window = windows.emplace_back(TWindow(getNewWindowId()));

                initializeWindow(window, windowParameters);

                return window;
            }
            
            virtual TWindow& getWindow(int id)
            {
                std::size_t size = windows.size();
                for(std::size_t i = 0; i < size; ++i)
                {
                    auto window = windows[i];
                    if(window.id == id)
                    {
                        return window;
                    }
                }
            }

            virtual void removeWindow(int id)
            {
                auto it = windows.begin();
                auto end = windows.end();

                for(; it != end; ++it)
                {
                    if((*it).id == id)
                    {
                        windows.erase(it);
                        break;
                    }
                }
            }

        protected:
            virtual int getNewWindowId()
            {
                return windows.size() + 1;
            }

            virtual void initializeWindow(TWindow& window, const TWindowParameters& windowParameters) = 0;
    };
}