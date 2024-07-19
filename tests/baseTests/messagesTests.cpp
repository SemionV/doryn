#include <catch2/catch_test_macros.hpp>

#include <dory/engine/domain/entity.h>
#include <dory/engine/domain/dataContext.h>
#include <dory/engine/domain/events/event.h>

using namespace dory::domain;

struct MessageData
{
    std::string message;
};

struct WindowClick
{
    public:
        const std::shared_ptr<dory::domain::entity::Window> window;
        const int x;
        const int y;

    public:
        WindowClick(std::shared_ptr<dory::domain::entity::Window> window, int x, int y):
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
        events::EventDispatcher<DataContext&, WindowClick&> clickEvent;
        events::EventDispatcher<DataContext&, KeyPressed&> keyPressedEvent;

    public:
        events::Event<DataContext&, WindowClick&>& onClick()
        {
            return clickEvent;
        }

        events::Event<DataContext&, KeyPressed&>& onKeyPressed()
        {
            return keyPressedEvent;
        }

    protected:
        events::EventDispatcher<DataContext&, WindowClick&>& onClickDispatcher()
        {
            return clickEvent;
        }

        events::EventDispatcher<DataContext&, KeyPressed&>& onKeyPressedDispatcher()
        {
            return keyPressedEvent;
        }
};

class WindowEventHubDispatcher: public WindowEventHub
{
    private:
        events::EventBuffer<DataContext, KeyPressed> keyPressedEventBuffer;
        events::EventBuffer<DataContext, WindowClick> clickEventBuffer;

    public:
        void addCase(WindowClick&& clickData)
        {
            clickEventBuffer.addCase(std::forward<WindowClick>(clickData));
        }

        void addCase(KeyPressed&& clickData)
        {
            keyPressedEventBuffer.addCase(std::forward<KeyPressed>(clickData));
        }

        void submit(DataContext& dataContext)
        {
            clickEventBuffer.submitCases(onClickDispatcher(), dataContext);
            keyPressedEventBuffer.submitCases(onKeyPressedDispatcher(), dataContext);
        }
};

TEST_CASE( "Event Hub", "[messages]" )
{
    WindowEventHubDispatcher eventHub;
    std::vector<WindowClick> clicks;
    dory::domain::DataContext dataContext;

    eventHub.onClick() += [&](dory::domain::DataContext& context, WindowClick& click)
    {
        clicks.push_back(click);
    };

    std::vector<WindowClick> clicks2;
    eventHub.onClick() += [&](dory::domain::DataContext& context, WindowClick& click)
    {
        clicks2.push_back(click);
    };

    std::vector<KeyPressed> keysPressed;
    eventHub.onKeyPressed() += [&](dory::domain::DataContext& context, KeyPressed& keyPressed)
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