#include "dependencies.h"

struct MessageData
{
    std::string message;
};

struct WindowClick
{
    public:
        const std::shared_ptr<dory::Window> window;
        const int x;
        const int y;

    public:
        WindowClick(std::shared_ptr<dory::Window> window, int x, int y):
            window(window),
            x(x),
            y(y)
        {
        }
};

struct KeyPressed
{
    public:
        const char key;

    public:
        KeyPressed(char key):
            key(key)
        {
        }
};

class WindowEventHub
{
    private:
        dory::EventDispatcher<dory::DataContext&, WindowClick&> clickEvent;
        dory::EventDispatcher<dory::DataContext&, KeyPressed&> keyPressedEvent;

    public:
        dory::Event<dory::DataContext&, WindowClick&>& onClick()
        {
            return clickEvent;
        }

        dory::Event<dory::DataContext&, KeyPressed&>& onKeyPressed()
        {
            return keyPressedEvent;
        }

    protected:
        dory::EventDispatcher<dory::DataContext&, WindowClick&>& onClickDispatcher()
        {
            return clickEvent;
        }

        dory::EventDispatcher<dory::DataContext&, KeyPressed&>& onKeyPressedDispatcher()
        {
            return keyPressedEvent;
        }
};

class WindowEventHubDispatcher: public WindowEventHub, public dory::EventHubDispatcher
{
    private:
        dory::EventBuffer<KeyPressed> keyPressedEventBuffer;
        dory::EventBuffer<WindowClick> clickEventBuffer;

    public:
        void addCase(WindowClick&& clickData)
        {
            clickEventBuffer.addCase(std::forward<WindowClick>(clickData));
        }

        void addCase(KeyPressed&& clickData)
        {
            keyPressedEventBuffer.addCase(std::forward<KeyPressed>(clickData));
        }

        void submit(dory::DataContext& dataContext) override
        {
            clickEventBuffer.submitCases(onClickDispatcher(), dataContext);
            keyPressedEventBuffer.submitCases(onKeyPressedDispatcher(), dataContext);
        }
};

TEST_CASE( "Event Hub", "[messages]" )
{
    WindowEventHubDispatcher eventHub;
    std::vector<WindowClick> clicks;
    dory::DataContext dataContext;

    eventHub.onClick() += [&](dory::DataContext& context, WindowClick& click)
    {
        clicks.push_back(click);
    };

    std::vector<WindowClick> clicks2;
    eventHub.onClick() += [&](dory::DataContext& context, WindowClick& click)
    {
        clicks2.push_back(click);
    };

    std::vector<KeyPressed> keysPressed;
    eventHub.onKeyPressed() += [&](dory::DataContext& context, KeyPressed& keyPressed)
    {
        keysPressed.push_back(keyPressed);
    };

    WindowClick click(nullptr, 2, 4);
    eventHub.addCase(std::forward<WindowClick>(click));

    WindowClick click2(nullptr, 3, 5);
    eventHub.addCase(std::forward<WindowClick>(click2));

    KeyPressed keyPressed(3);
    eventHub.addCase(std::forward<KeyPressed>(keyPressed));

    eventHub.submit(dataContext);
    eventHub.submit(dataContext);//submit second time to make sure that the buffers are clean

    REQUIRE(clicks.size() == 2);
    REQUIRE(clicks[0].x == 2);
    REQUIRE(clicks[0].y == 4);
    REQUIRE(clicks[1].x == 3);
    REQUIRE(clicks[1].y == 5);

    REQUIRE(clicks2.size() == 2);
    REQUIRE(clicks2[0].x == 2);
    REQUIRE(clicks2[0].y == 4);
    REQUIRE(clicks2[1].x == 3);
    REQUIRE(clicks2[1].y == 5);

    REQUIRE(keysPressed.size() == 1);
    REQUIRE(keysPressed[0].key == 3);
}