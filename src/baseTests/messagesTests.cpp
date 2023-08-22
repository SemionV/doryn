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

class WindowEventsHub
{
    protected:
        dory::EventHubDispatcher<WindowClick> clickEventHub;

    public:
        dory::Event<WindowClick&>& onClick()
        {
            return clickEventHub.getEvent();
        }
};

class WindowEventsHubDispatcher: public WindowEventsHub, public dory::EventsHubDispatcher
{
    public:
        dory::EventHubDispatcher<WindowClick>& onClickDispatcher()
        {
            return clickEventHub;
        }

        void submit() override
        {
            clickEventHub.submitCases();
        }
};

TEST_CASE( "Event Hub", "[messages]" )
{
    WindowEventsHubDispatcher eventHub;
    std::vector<WindowClick> clicks;

    eventHub.onClick().attachHandler([&](WindowClick& click)
    {
        clicks.push_back(click);
    });

    std::vector<WindowClick> clicks2;
    eventHub.onClick().attachHandler([&](WindowClick& click)
    {
        clicks2.push_back(click);
    });

    WindowClick click(nullptr, 2, 4);
    eventHub.onClickDispatcher().addCase(std::forward<WindowClick>(click));

    WindowClick click2(nullptr, 3, 5);
    eventHub.onClickDispatcher().addCase(std::forward<WindowClick>(click2));

    eventHub.submit();

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
}