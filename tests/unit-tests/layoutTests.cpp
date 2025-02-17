#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <dory/core/resources/scene/configuration.h>

using namespace dory;
using namespace dory::core;
using namespace dory::core::resources;
using namespace dory::core::resources::scene;
using namespace dory::core::resources::scene::configuration;

TEST(LayoutTests, gridLayout)
{
    layout::Container layoutContainer {"root"};
    layoutContainer.position = {}; //get automatically positioned(it should be {0,0})
    layoutContainer.size = {}; //get the full size of the parent surface(should be {parentWidth, parentHeight})

    layout::Container row1Container {"row1"};
    row1Container.position = {};
    row1Container.size = { {}, layout::Dimension{100}};

    layout::Container row2Container {"row2"};
    row1Container.position = {};
    row1Container.size = { {}, layout::Dimension{100}};

    layout::Container row3Container {"row3"};
    row1Container.position = {};
    row1Container.size = { {}, layout::Dimension{100}};

    layout::Container row4Container {"row4"};
    row1Container.position = {};
    row1Container.size = {}; //take the rest of the parents available space

    layoutContainer.containers = { row1Container, row2Container, row3Container, row4Container };
}