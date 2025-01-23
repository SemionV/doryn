#include <refl.hpp>
#include <chrono>
#include <iostream>
#include <gtest/gtest.h>
#include <gmock/gmock.h>

class IController
{
public:
    virtual ~IController() = default;

    virtual void update(std::chrono::nanoseconds deltaTime) = 0;
};

class SomeController: public IController
{
public:
    void update(std::chrono::nanoseconds deltaTime) override
    {
        std::cout << "SomeController::update" << std::endl;
    }
};

REFL_AUTO (
    type(SomeController)
)

struct Controllers
{
    SomeController SomeController;
};

REFL_TYPE(Controllers)
    REFL_FIELD(SomeController)
REFL_END

TEST(ControllerFactory, createByName)
{
    constexpr std::string_view typeName = "SomeController";
    std::shared_ptr<IController> controllerInstance;

    auto typeDescriptor = refl::reflect<Controllers>();

    for_each(typeDescriptor.members, [&]<typename MemberDescriptor>(MemberDescriptor memberDescriptor)
    {
        if constexpr (is_field(memberDescriptor))
        {
            const auto memberName = std::string_view{MemberDescriptor::name.data, MemberDescriptor::name.size};
            if(memberName == typeName)
            {
                controllerInstance = std::make_shared<typename MemberDescriptor::value_type>();
            }
        }
    });

    EXPECT_TRUE(controllerInstance != nullptr);
    controllerInstance->update(std::chrono::nanoseconds { 1 });
}