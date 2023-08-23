#include "dependencies.h"

struct MessageData
{
    std::string message;
};

TEST_CASE( "Create a simple Message", "[messages]" )
{
    dory::MessageFactory messageFactory;

    std::shared_ptr<dory::Message> message = messageFactory.createMessage(dory::MessageType::MouseTestMessage);

    REQUIRE(message);
    REQUIRE(message->messageType == dory::MessageType::MouseTestMessage);
    
    auto windowMessage = messageFactory.createMessage<dory::WindowMessage>(dory::MessageType::MouseTestMessage, nullptr);

    MessageData messageData;
    messageData.message = "Test data";
    
    auto dataMessage = messageFactory.createMessage<dory::DataMessage<MessageData>>(dory::MessageType::MouseTestMessage, std::forward<MessageData>(messageData));

    REQUIRE(dataMessage);
    REQUIRE(dataMessage->messageType == dory::MessageType::MouseTestMessage);
    REQUIRE(dataMessage->data.message == "Test data");

    std::shared_ptr<dory::Message> dataMessagePtr = messageFactory.createMessage<dory::DataMessage<MessageData>>(dory::MessageType::MouseTestMessage, std::forward<MessageData>(messageData));

    REQUIRE(dataMessagePtr);
    REQUIRE(dataMessagePtr->messageType == dory::MessageType::MouseTestMessage);
}

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
        dory::EventDispatcher<WindowClick&> clickEvent;
        dory::EventDispatcher<KeyPressed&> keyPressedEvent;

    public:
        dory::Event<WindowClick&>& onClick()
        {
            return clickEvent;
        }

        dory::Event<KeyPressed&>& onKeyPressed()
        {
            return keyPressedEvent;
        }

    protected:
        dory::EventDispatcher<WindowClick&>& onClickDispatcher()
        {
            return clickEvent;
        }

        dory::EventDispatcher<KeyPressed&>& onKeyPressedDispatcher()
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

        void submit() override
        {
            clickEventBuffer.submitCases(onClickDispatcher());
            keyPressedEventBuffer.submitCases(onKeyPressedDispatcher());
        }
};

TEST_CASE( "Event Hub", "[messages]" )
{
    WindowEventHubDispatcher eventHub;
    std::vector<WindowClick> clicks;

    eventHub.onClick() += [&](WindowClick& click)
    {
        clicks.push_back(click);
    };

    std::vector<WindowClick> clicks2;
    eventHub.onClick() += [&](WindowClick& click)
    {
        clicks2.push_back(click);
    };

    std::vector<KeyPressed> keysPressed;
    eventHub.onKeyPressed() += [&](KeyPressed& keyPressed)
    {
        keysPressed.push_back(keyPressed);
    };

    WindowClick click(nullptr, 2, 4);
    eventHub.addCase(std::forward<WindowClick>(click));

    WindowClick click2(nullptr, 3, 5);
    eventHub.addCase(std::forward<WindowClick>(click2));

    KeyPressed keyPressed(3);
    eventHub.addCase(std::forward<KeyPressed>(keyPressed));

    eventHub.submit();
    eventHub.submit();//submit second time to make sure that the buffers are clean

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