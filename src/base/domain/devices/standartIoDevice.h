#pragma once

#include "base/typeComponents.h"
#include "base/domain/events/ioEventHub.h"

namespace dory::domain::devices
{
    template<typename TImplementation, typename TDataContext>
    struct IDevice: Uncopyable, public StaticInterface<TImplementation>
    {
        void connect(TDataContext& context)
        {
            this->toImplementation()->connectImpl();
        }

        void disconnect(TDataContext& context)
        {
            this->toImplementation()->disconnectImpl();
        }
    };

    template<typename TDataContext>
    class StandartInputOutputDeviceWin32
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

        void onOutput(TDataContext& context, const std::string& eventData)
        {
            std::cout << eventData;
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
        }

        void disconnectImpl(TDataContext& context)
        {
            pollingThread.request_stop();
        }
    };
}
