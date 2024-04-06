#pragma once

#include "device.h"
#include "base/domain/events/ioEventHub.h"

namespace dory::domain::devices
{
    template<typename TDataContext>
    class StandartInputOutputDeviceWin32: public IDevice<StandartInputOutputDeviceWin32<TDataContext>, TDataContext>
    {
    private:
        using IOEventHubDispatcherType =  events::IOEventHubDispatcher<TDataContext, int, std::string>;
        IOEventHubDispatcherType& eventHubDispatcher;

        std::jthread pollingThread;

    protected:
        void onKeyPressed(int key)
        {
            eventHubDispatcher.addCase(key);
        }

        void onOutput(TDataContext& context, std::string& eventData)
        {
            std::cout << eventData;
        }

        void onFlush(TDataContext& context, const events::FlushOutputBuffer flushOutputBuffer)
        {
            std::cout.flush();
        }

    public:
        explicit StandartInputOutputDeviceWin32(IOEventHubDispatcherType& eventHubDispatcher):
                eventHubDispatcher(eventHubDispatcher)
        {}

        void connectImpl(TDataContext& context)
        {
            pollingThread = std::jthread([this](const std::stop_token& stoken)
            {
                while(!stoken.stop_requested())
                {
                    int inputKey = getchar();

                    if(!stoken.stop_requested())
                    {
                        onKeyPressed(inputKey);
                    }
                }
            });

            eventHubDispatcher.onOutput().attachHandler(this, &StandartInputOutputDeviceWin32::onOutput);
            eventHubDispatcher.onFlush().attachHandler(this, &StandartInputOutputDeviceWin32::onFlush);
        }

        void disconnectImpl(TDataContext& context)
        {
            pollingThread.request_stop();
            pollingThread.detach();
        }
    };
}
