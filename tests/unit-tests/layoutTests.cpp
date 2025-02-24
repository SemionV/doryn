#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <dory/core/resources/scene/configuration.h>
#include <dory/core/resources/objects/layout.h>
#include <dory/core/services/layoutService.h>

using namespace dory;
using namespace dory::core;
using namespace dory::core::resources;
using namespace dory::core::resources::scene;
using namespace dory::core::resources::scene::configuration;

TEST(LayoutTests, layoutControl)
{
    layout::Container container1;

    //set size to available space in the parent
    container1.size = {};

    //set size by size of the content, both width and height
    container1.size = layout::Size{};

    container1.size = layout::Size {};
    //set width to available width in the parent
    container1.size->width = {};
    //set height by the height of the content
    container1.size->height = layout::Dimension{};
    container1.size->height->percents = {};
    container1.size->height->pixels = {};
    //set height to explicit value
    container1.size->height = layout::Dimension{};
    container1.size->height->percents = {};
    container1.size->height->pixels = { 100 };
}

TEST(LayoutTests, centeredPosition)
{
    layout::PositionedContainer root {};
    root.name = "root";

    root.size = layout::Size {
        layout::Dimension { 150 },
        layout::Dimension { 100 }
    };

    services::LayoutService layoutService;
    const objects::layout::Container container = layoutService.calculate(root, objects::layout::Size{ 350, 400 });

    EXPECT_EQ(container.name, root.name);
    EXPECT_EQ(container.size.width, 150);
    EXPECT_EQ(container.size.height, 100);
    EXPECT_EQ(container.position.x, 100);
    EXPECT_EQ(container.position.y, 150);
}

TEST(LayoutTests, originPosition)
{
    layout::PositionedContainer root {};
    root.name = "root";

    root.position = layout::Position {
        layout::Dimension {},
        layout::Dimension {}
    };
    root.size = layout::Size {
        layout::Dimension { 150 },
        layout::Dimension { 100 }
    };

    services::LayoutService layoutService;
    const objects::layout::Container container = layoutService.calculate(root, objects::layout::Size{ 350, 400 });

    EXPECT_EQ(container.name, root.name);
    EXPECT_EQ(container.size.width, 150);
    EXPECT_EQ(container.size.height, 100);
    EXPECT_EQ(container.position.x, 0);
    EXPECT_EQ(container.position.y, 0);
}

TEST(LayoutTests, explicitPosition)
{
    layout::PositionedContainer root {};
    root.name = "root";

    root.position = layout::Position {
        layout::Dimension { 12 },
        layout::Dimension { 225 }
    };
    root.size = layout::Size {
        layout::Dimension { 150 },
        layout::Dimension { 100 }
    };

    services::LayoutService layoutService;
    const objects::layout::Container container = layoutService.calculate(root, objects::layout::Size{ 350, 400 });

    EXPECT_EQ(container.name, root.name);
    EXPECT_EQ(container.size.width, 150);
    EXPECT_EQ(container.size.height, 100);
    EXPECT_EQ(container.position.x, 12);
    EXPECT_EQ(container.position.y, 225);
}

TEST(LayoutTests, fullScreen)
{
    layout::PositionedContainer root {};
    root.name = "root";

    services::LayoutService layoutService;
    constexpr objects::layout::Size availableSpace{ 1024, 768 };
    const objects::layout::Container container = layoutService.calculate(root, availableSpace);

    EXPECT_EQ(container.name, root.name);
    EXPECT_EQ(container.size.width, availableSpace.width);
    EXPECT_EQ(container.size.height, availableSpace.height);
    EXPECT_EQ(container.position.x, 0);
    EXPECT_EQ(container.position.y, 0);
}