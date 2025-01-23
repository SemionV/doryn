#include <refl.hpp>
#include <chrono>
#include <iostream>
#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <magic_enum/magic_enum.hpp>

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

// 1) Define your enum class
enum class Color
{
    Red,
    Green,
    Blue
};

enum class ToneMapping {
#define IKAROS_ENUM_ToneMapping \
	IKAROS_ENUM_E(None) \
	IKAROS_ENUM_E(Uncharted2) \
	IKAROS_ENUM_E(ACES)
#define IKAROS_ENUM_E(_entry) _entry,

	IKAROS_ENUM_ToneMapping

#undef IKAROS_ENUM_E
};

template<typename TEnum>
struct EnumTypeDescriptor
{
    constexpr static std::array<std::pair<TEnum, const char *>, 0> lookupTable = {};
};

template<>
struct EnumTypeDescriptor<ToneMapping>
{
    constexpr static std::array lookupTable = {
#define IKAROS_ENUM_E(_entry) std::pair{ToneMapping::_entry, #_entry},
        IKAROS_ENUM_ToneMapping
#undef IKAROS_ENUM_E
    };
};

template<typename TEnum>
constexpr std::optional<TEnum> fromString(std::string_view s)
{
    for (auto const& [enumVal, enumName] : EnumTypeDescriptor<TEnum>::lookupTable)
    {
        if (enumName == s)
        {
            return enumVal;
        }
    }

    return {};
}

template<typename TEnum>
constexpr std::string_view toString(TEnum value)
{
    for (auto const& [enumVal, enumName] : EnumTypeDescriptor<TEnum>::lookupTable)
    {
        if (enumVal == value)
        {
            return enumName;
        }
    }
    return "Unknown";
}

constexpr std::optional<ToneMapping> choice2 = fromString<ToneMapping>("ACES");
constexpr std::string_view name = toString(ToneMapping::Uncharted2);

TEST(Enums, reflection)
{
    constexpr Color optionValue = magic_enum::enum_cast<Color>("Green").value();
    constexpr std::string_view optionName = magic_enum::enum_name(Color::Blue);

    EXPECT_EQ(optionValue, Color::Green);
    EXPECT_EQ(optionName, "Blue");
}