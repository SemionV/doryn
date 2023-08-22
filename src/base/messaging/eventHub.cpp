#include "base/dependencies.h"
#include "eventHub.h"

namespace dory
{
    void WindowEventHubDispatcher::submit()
    {
        EventDispatcher<WindowClick&>& windowClickDispatcher = getWindowClickDispatcher();
        for(std::size_t i = 0; i < windowClicks.size(); i++)
        {
            try
            {
                windowClickDispatcher(windowClicks[i]);
            }
            catch(const std::exception& e)
            {                
            }
        }
        windowClicks.clear();
    }
}