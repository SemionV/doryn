#include "dependencies.h"

TEST_CASE( "Create a simple Message", "[messages]" ) {
    dory::MessageFactory messageFactory;

    std::shared_ptr<dory::Message> message = messageFactory.createMessage(dory::MessageType::MouseTestMessage);

    REQUIRE(message);
    REQUIRE(message->messageType == dory::MessageType::MouseTestMessage);
    
    auto windowMessage = messageFactory.createMessage<dory::WindowMessage>(dory::MessageType::MouseTestMessage, nullptr);
}