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

TEST(LayoutTests, percentDimensions)
{
    layout::PositionedContainer root {};
    root.name = "root";
    root.size = layout::Size {
        layout::Dimension { {}, {10.f} },
        layout::Dimension { {}, {20.f} }
    };
    root.position = layout::Position {
        layout::Dimension { {}, {30.f} },
        layout::Dimension { {}, {10.f} }
    };

    services::LayoutService layoutService;
    constexpr objects::layout::Size availableSpace{ 1000, 1000 };
    const objects::layout::Container container = layoutService.calculate(root, availableSpace);

    EXPECT_EQ(container.name, root.name);
    EXPECT_EQ(container.size.width, 100);
    EXPECT_EQ(container.size.height, 200);
    EXPECT_EQ(container.position.x, 300);
    EXPECT_EQ(container.position.y, 100);
}

void assertContainer(const objects::layout::Container& container, const Name& name, const std::size_t x, const std::size_t y,
    const std::size_t width, const std::size_t height)
{
    EXPECT_EQ(container.size.width, width);
    EXPECT_EQ(container.size.height, height);
    EXPECT_EQ(container.position.x, x);
    EXPECT_EQ(container.position.y, y);
}

TEST(LayoutTests, gridTwoColumnsLine)
{
    layout::PositionedContainer root {};
    root.name = "root";

    layout::FloatingContainer& column1 = root.horizontal.emplace_back();
    column1.size = layout::Size { layout::Dimension{ 124 }, {} }; //124px width, 100% height

    layout::FloatingContainer& column2 = root.horizontal.emplace_back(); //100% - 124px width, 100% height

    services::LayoutService layoutService;
    constexpr objects::layout::Size availableSpace{ 1024, 768 };
    const objects::layout::Container container = layoutService.calculate(root, availableSpace);

    EXPECT_EQ(container.name, root.name);
    EXPECT_EQ(container.children.size(), 2);

    assertContainer(container.children[0], column1.name, 0, 0, 124, availableSpace.height);
    assertContainer(container.children[1], column2.name, 124, 0, 900, availableSpace.height);
}

TEST(LayoutTests, gridTwoRowsLine)
{
    layout::PositionedContainer root {};
    root.name = "root";

    layout::FloatingContainer& row1 = root.vertical.emplace_back();
    row1.size = layout::Size { {}, layout::Dimension{ 168 } };

    layout::FloatingContainer& row2 = root.vertical.emplace_back();

    services::LayoutService layoutService;
    constexpr objects::layout::Size availableSpace{ 1024, 768 };
    const objects::layout::Container container = layoutService.calculate(root, availableSpace);

    EXPECT_EQ(container.name, root.name);
    EXPECT_EQ(container.children.size(), 2);

    assertContainer(container.children[0], row1.name, 0, 0, availableSpace.width, 168);
    assertContainer(container.children[1], row2.name, 0, 168, availableSpace.width, 600);
}

TEST(LayoutTests, gridThreeColumnsLine)
{
    layout::PositionedContainer root {};
    root.name = "root";

    layout::FloatingContainer& column1 = root.horizontal.emplace_back();
    column1.size = layout::Size { layout::Dimension{ 124 }, {} };

    layout::FloatingContainer& column2 = root.horizontal.emplace_back();

    layout::FloatingContainer& column3 = root.horizontal.emplace_back();
    column3.size = layout::Size { layout::Dimension{ 100 }, {} };

    services::LayoutService layoutService;
    constexpr objects::layout::Size availableSpace{ 1024, 768 };
    const objects::layout::Container container = layoutService.calculate(root, availableSpace);

    EXPECT_EQ(container.name, root.name);
    EXPECT_EQ(container.children.size(), 3);

    assertContainer(container.children[0], column1.name, 0, 0, 124, availableSpace.height);
    assertContainer(container.children[1], column2.name, 124, 0, 800, availableSpace.height);
    assertContainer(container.children[2], column3.name, 924, 0, 100, availableSpace.height);
}

TEST(LayoutTests, gridThreeRowsLine)
{
    layout::PositionedContainer root {};
    root.name = "root";

    layout::FloatingContainer& row1 = root.vertical.emplace_back();
    row1.size = layout::Size { {}, layout::Dimension{ 168 } };

    layout::FloatingContainer& row2 = root.vertical.emplace_back();

    layout::FloatingContainer& row3 = root.vertical.emplace_back();
    row3.size = layout::Size { {}, layout::Dimension{ 100 } };

    services::LayoutService layoutService;
    constexpr objects::layout::Size availableSpace{ 1024, 768 };
    const objects::layout::Container container = layoutService.calculate(root, availableSpace);

    EXPECT_EQ(container.name, root.name);
    EXPECT_EQ(container.children.size(), 3);

    assertContainer(container.children[0], row1.name, 0, 0, availableSpace.width, 168);
    assertContainer(container.children[1], row2.name, 0, 168, availableSpace.width, 500);
    assertContainer(container.children[2], row3.name, 0, 668, availableSpace.width, 100);
}