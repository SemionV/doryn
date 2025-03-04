#include <dory/core/services/layoutService2.h>
#include <cmath>

namespace dory::core::services
{
    using namespace resources;
    using namespace resources::scene;
    using namespace resources::scene::configuration;

    int getValue(const objects::layout::DimensionValue& value, const int parentSize, const objects::layout::Variables& variables)
    {
        int result {};

        if(value.pixels)
        {
            result = *value.pixels;
        }
        else if(value.percents)
        {
            result = static_cast<int>(std::round(*value.percents * 0.01f * static_cast<float>(parentSize)));
        }
        else if(value.variable)
        {
            //TODO: read value from a variable
        }

        return result;
    }

    int getSizeValue(const objects::layout::StretchingAxis& axis, const objects::layout::Size& parentSize,
        const objects::layout::Size& parentContentSize, const objects::layout::Variables& variables)
    {
        int result {};

        const auto value = axis.value;
        if(value.upstream == objects::layout::Upstream::self)
        {
            result = getValue(value, parentSize.*axis.property, variables);
        }
        else if(value.upstream == objects::layout::Upstream::parent)
        {
            result = parentSize.*axis.property - parentContentSize.*axis.property;
        }

        return result;
    }

    int getPositionValue(const objects::layout::AlignmentAxis& xAxis, const objects::layout::AlignmentAxis& yAxis,
        const objects::layout::StretchingAxis& widthAxis, const objects::layout::StretchingAxis& heightAxis,
        objects::layout::LineCursor& cursor, const objects::layout::Size& size, const objects::layout::Size& parentSize,
        const objects::layout::Variables& variables)
    {
        int result {};

        const auto& nodeWidth = size.*widthAxis.property;
        const auto& nodeHeight = size.*widthAxis.property;
        const auto& parentWidth = parentSize.*widthAxis.property;

        auto& upperLeftCursorY = cursor.upperLeftCorner.*yAxis.property;
        auto& bottomRightCursorX = cursor.bottomRightCorner.*xAxis.property;
        auto& bottomRightCursorY = cursor.bottomRightCorner.*yAxis.property;

        if(xAxis.order == objects::layout::AlignOrder::line)
        {
            if(yAxis.order == objects::layout::AlignOrder::wrap && bottomRightCursorX + nodeWidth > parentWidth)
            {
                upperLeftCursorY += bottomRightCursorY - upperLeftCursorY;
                bottomRightCursorY += upperLeftCursorY;
                bottomRightCursorX = 0;
            }

            result = bottomRightCursorX;
            bottomRightCursorX += nodeWidth;

            if(nodeHeight > bottomRightCursorY - upperLeftCursorY)
            {
                bottomRightCursorY = upperLeftCursorY + nodeHeight;
            }
        }
        else if(xAxis.order == objects::layout::AlignOrder::wrap)
        {
            result = upperLeftCursorY;
        }
        else if(xAxis.order == objects::layout::AlignOrder::center)
        {
            result = static_cast<int>(std::round(static_cast<float>(parentWidth - nodeWidth) / 2.f));
        }
        else if(xAxis.order == objects::layout::AlignOrder::relative)
        {
            result = getValue(xAxis.value, parentWidth, variables);
        }

        return result;
    }

    objects::layout::NodeStateList buildNodeList(const objects::layout::NodeSetupList& setupList)
    {
        objects::layout::NodeStateList list;
        list.nodes.reserve(setupList.nodes.size());
        return list;
    }

    void propagateValue(std::size_t nodeIndex, std::size_t parentIndex, const objects::layout::StretchingAxisProperty axisProperty,
        const objects::layout::NodeSetupList& setupList, objects::layout::NodeStateList& stateList)
    {
        const auto& nodeSetup = setupList.nodes[nodeIndex];
        const auto& nodeState = stateList.nodes[nodeIndex];
        const objects::layout::StretchingAxis& axis = nodeSetup.stretching.axes.*axisProperty;

        if(axis.valuePropagation)
        {
            const int value = nodeState.size.*axis.property;

            while(nodeIndex != parentIndex) //the first node on the list has parent index pointing on himself
            {
                auto& parentNodeSetup = setupList.nodes[parentIndex];
                auto& parentNodeState = stateList.nodes[parentIndex];
                const objects::layout::StretchingAxis& parentAxis = parentNodeSetup.stretching.axes.*axisProperty;

                parentNodeState.contentSize.*parentAxis.property += value;

                if(parentAxis.value.upstream == objects::layout::Upstream::children)
                {
                    parentNodeState.size.*parentAxis.property += value;
                }
                else
                {
                    break;
                }

                if(!parentAxis.valuePropagation)
                {
                    break;
                }

                nodeIndex = parentIndex;
                parentIndex = parentNodeSetup.parent;
            }
        }
    }

    void calculateSizes(const objects::layout::NodeSetupList& setupList, objects::layout::NodeStateList& stateList, const objects::layout::Variables& variables)
    {
        for(std::size_t i = 0; i < setupList.nodes.size(); ++i)
        {
            auto& nodeSetup = setupList.nodes[i];
            auto& nodeState = stateList.nodes[i];
            auto& parentNodeState = stateList.nodes[nodeSetup.parent];

            const auto& stretching = nodeSetup.stretching;

            auto& width = nodeState.size.*stretching.axes.width.property;
            auto& height = nodeState.size.*stretching.axes.height.property;

            width = getSizeValue(stretching.axes.width, parentNodeState.size, parentNodeState.contentSize, variables);
            height = getSizeValue(stretching.axes.height, parentNodeState.size, parentNodeState.contentSize, variables);

            propagateValue(i, nodeSetup.parent, &objects::layout::StretchingAxes::width, setupList, stateList);
            propagateValue(i, nodeSetup.parent, &objects::layout::StretchingAxes::height, setupList, stateList);
        }
    }

    void calculatePositions(const objects::layout::NodeSetupList& setupList, objects::layout::NodeStateList& stateList, const objects::layout::Variables& variables)
    {
        for(std::size_t i = 0; i < setupList.nodes.size(); ++i)
        {
            auto& nodeSetup = setupList.nodes[i];
            auto& nodeState = stateList.nodes[i];

            for(const auto j : nodeSetup.children)
            {
                auto& childNodeSetup = setupList.nodes[j];
                auto& childNodeState = stateList.nodes[j];

                const auto& xAxis = childNodeSetup.alignment.axes.x;
                const auto& yAxis = childNodeSetup.alignment.axes.y;
                const auto& widthAxis = childNodeSetup.stretching.axes.width;
                const auto& heightAxis = childNodeSetup.stretching.axes.height;

                childNodeState.position.x = getPositionValue(xAxis, yAxis, widthAxis, heightAxis, nodeState.cursor, childNodeState.size, nodeState.size, variables);
                childNodeState.position.y = getPositionValue(yAxis, xAxis, heightAxis, widthAxis, nodeState.cursor, childNodeState.size, nodeState.size, variables);
            }
        }
    }

    std::unique_ptr<objects::layout::Container> buildContainer(const objects::layout::NodeSetupList& setupList, const objects::layout::NodeStateList& stateList)
    {
        std::unique_ptr<objects::layout::Container> rootContainer {};
        std::vector<objects::layout::Container*> lookupTable(stateList.nodes.size());

        for(std::size_t i = 0; i < stateList.nodes.size(); ++i)
        {
            const auto& nodeState = stateList.nodes[i];
            const auto& nodeSetup = setupList.nodes[i];

            objects::layout::Container* container {};

            if(nodeSetup.parent != i)
            {
                auto parentNodeSetup = setupList.nodes[nodeSetup.parent];
                for(std::size_t j = 0; j < parentNodeSetup.children.size(); ++j)
                {
                    if(parentNodeSetup->children[j] == i)
                    {
                        const auto parentContainer = lookupTable[nodeSetup.parent];
                        container = lookupTable[i] = &parentContainer->children[j];
                        break;
                    }
                }
            }
            else
            {
                //root node
                rootContainer = std::make_unique<objects::layout::Container>();
                lookupTable[i] = container = rootContainer.get();
            }

            if(container)
            {
                container->children.reserve(nodeSetup.children.size());
                container->name = nodeSetup.name;
                container->position = nodeState.position;
                container->size = nodeState.size;
            }
        }

        return rootContainer;
    }

    std::unique_ptr<objects::layout::Container> LayoutService2::calculate(const objects::layout::NodeSetupList& setupList, const objects::layout::Variables& variables)
    {
        objects::layout::NodeStateList stateList = buildNodeList(setupList);
        calculateSizes(setupList, stateList, variables);
        calculatePositions(setupList, stateList, variables);

        return buildContainer(setupList, stateList);
    }
}

