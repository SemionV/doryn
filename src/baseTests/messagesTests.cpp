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

TEST_CASE( "Event Hub", "[messages]" )
{
    dory::WindowEventHubDispatcher eventHub;

    bool clickHandle = false;
    int x = 0, y = 0;

    eventHub.getWindowClick().attachHandler([&](dory::WindowClick& click)
    {
        clickHandle = true;
        x = click.x;
        y = click.y;
    });

    bool clickHandle2 = false;
    int x2 = 0, y2 = 0;
    eventHub.getWindowClick().attachHandler([&](dory::WindowClick& click)
    {
        clickHandle2 = true;
        x2 = click.x;
        y2 = click.y;
    });

    dory::WindowClick click(nullptr, 2, 4);
    eventHub.onWindowClick(std::forward<dory::WindowClick>(click));

    dory::WindowClick click2(nullptr, 3, 5);
    eventHub.onWindowClick(std::forward<dory::WindowClick>(click2));

    eventHub.submit();

    REQUIRE(clickHandle);
    REQUIRE(x == 3);
    REQUIRE(y == 5);

    REQUIRE(clickHandle2);
    REQUIRE(x2 == 3);
    REQUIRE(y2 == 5);
}