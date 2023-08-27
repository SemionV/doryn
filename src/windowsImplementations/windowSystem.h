#pragma once

#include "windowParameters.h"
#include "window.h"

namespace doryWindows
{
    LRESULT WINAPI windowProcedure(HWND hWnd, UINT WindowsMessage, WPARAM wParam, LPARAM lParam);

    class DORY_API WindowSystem: public dory::WindowSystem<WindowParameters>
    {
        private:
            std::shared_ptr<dory::SystemWindowEventHubDispatcher> eventHub;

        public:
            WindowSystem(std::shared_ptr<dory::SystemWindowEventHubDispatcher> eventHub):
                eventHub(eventHub)
            {
            }

            virtual bool connect() override;
            virtual void disconnect() override;
            virtual void update() override;
            virtual std::shared_ptr<dory::Window> createWindow(const WindowParameters& parameters) override;
            
            virtual void onClick(HWND hWnd, int x, int y);

        protected:
            virtual void pumpSystemMessages();
            std::shared_ptr<Window> getWindow(HWND hWnd);
    };
}