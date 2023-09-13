#pragma once

#include "windowParameters.h"
#include "win32Window.h"

namespace doryWindows
{
    LRESULT WINAPI windowProcedure(HWND hWnd, UINT WindowsMessage, WPARAM wParam, LPARAM lParam);

    class DORY_API WindowSystem: public dory::WindowSystem<WindowParameters, Win32Window>
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
            void submitEvents(dory::DataContext& context) override;
            virtual std::shared_ptr<Win32Window> createWindow(const WindowParameters& parameters) override;
            virtual void closeWindow(std::shared_ptr<Win32Window> window) override;

            virtual void onClick(HWND hWnd, int x, int y);

        protected:
            virtual void pumpSystemMessages();
            std::shared_ptr<Win32Window> getWindow(HWND hWnd);
    };
}