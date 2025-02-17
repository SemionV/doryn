#include <dory/core/services/layoutService.h>

namespace dory::core::services
{
    using namespace resources;
    using namespace resources::scene;
    using namespace resources::scene::configuration;

    layout::Position getAvailablePosition(layout::Container& parentContainer, const std::size_t index)
    {
        layout::Position position { layout::Dimension{ 0 }, layout::Dimension{ 0 } };

        //TODO: travers the child containers and find next available position

        return position;
    }

    layout::Size getAvailableSize(layout::Container& parentContainer, const layout::Position& position )
    {
        assert(parentContainer.size.has_value());
        layout::Size size { *parentContainer.size };

        //TODO: calculate free space available for position

        return size;
    }

    void calculateContainer(layout::Container& parentContainer, const layout::Container& containerDefinition, const std::size_t index)
    {
        layout::Container& container = parentContainer.containers[index];

        //consider container's size by definition
        if(containerDefinition.size.has_value())
        {

        }
        //use available space to set size of the container
        else
        {
            const layout::Position availablePosition = getAvailablePosition(parentContainer, index);
            layout::Size availableSize = getAvailableSize(parentContainer, availablePosition);

            container.position = availablePosition;
            container.size = availableSize;
        }
    }

    layout::Container LayoutService::calculate(const layout::Container& container, const layout::Size& constraintSize)
    {
        layout::Container result {container.name};
        layout::Container parentContainer { "", {}, constraintSize, { result } };

        calculateContainer(parentContainer, container, 0);

        return result;
    }
}
