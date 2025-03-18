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

    int getAlignmentValue(const objects::layout::PositionValue& value, const int size, const int parentSize, const objects::layout::Variables& variables)
    {
        if(value.order)
        {
            if(value.order.value() == objects::layout::AlignOrder::center)
            {
                return static_cast<int>(std::round(static_cast<float>(parentSize - size) / 2.f));
            }
        }

        return getValue(value, parentSize, variables);
    }

    int getSizeValue(const objects::layout::StretchingAxis& axis, const objects::layout::AlignmentAxis& aAxis,
        const objects::layout::NodeItemState& parentState, const objects::layout::Variables& variables)
    {
        int result {};

        const auto value = axis.value;
        if(value.upstream)
        {
            if(*value.upstream == objects::layout::Upstream::fill)
            {
                result = parentState.size.*axis.property - parentState.cursor.bottomRightCorner.*aAxis.property;
            }
            else if(*value.upstream == objects::layout::Upstream::parent)
            {
                result = parentState.size.*axis.property;
            }
        }
        else
        {
            result = getValue(value, parentState.size.*axis.property, variables);
        }

        return result;
    }

    objects::layout::NodeStateList buildNodeList(const objects::layout::NodeSetupList& setupList)
    {
        objects::layout::NodeStateList list;
        list.nodes.resize(setupList.nodes.size());
        return list;
    }

    void setCursorToNode(const objects::layout::NodeSetupList& setupList, objects::layout::NodeStateList& stateList, std::size_t nodeIndex)
    {
        const auto& nodeSetup = setupList.nodes[nodeIndex];
        if(nodeSetup.parent != nodeIndex)
        {
            const auto& nodeState = stateList.nodes[nodeIndex];
            auto& parentNodeState = stateList.nodes[nodeSetup.parent];

            objects::layout::LineCursor& cursor = parentNodeState.cursor;

            switch(nodeSetup.alignment.strategy)
            {
            case objects::layout::AlignmentStrategy::horizontalLine:
            case objects::layout::AlignmentStrategy::horizontalTiles:
                {
                    cursor.bottomRightCorner.x = nodeState.position.x;
                    break;

                }
            case objects::layout::AlignmentStrategy::verticalLine:
            case objects::layout::AlignmentStrategy::verticalTiles:
                {
                    cursor.bottomRightCorner.y = nodeState.position.y;
                    break;
                }
            default: break;
            }
        }
    }

    void toPosition(const objects::layout::Vector2i& position, objects::layout::Vector2i& cursor)
    {
        for(std::size_t i = 0; i < cursor.size(); ++i)
        {
            cursor[i] = position[i];
        }
    }

    void toOrigin(objects::layout::Vector2i& cursor)
    {
        toPosition({0, 0}, cursor);
    }

    void toOrigin(objects::layout::LineCursor& cursor)
    {
        toOrigin(cursor.ul);
        toOrigin(cursor.br);
    }

    void alignToOrigin(objects::layout::NodeItemState& nodeState, objects::layout::LineCursor& cursor)
    {
        toOrigin(cursor);
        toPosition(cursor.br, nodeState.pos);
        toPosition(nodeState.dim, cursor.br);
    }

    void alignToLine(const objects::layout::Strategy& strategy, objects::layout::NodeItemState& nodeState, objects::layout::LineCursor& cursor)
    {
        auto& axes = strategy.axes;
        auto x = axes[0];
        auto y = axes[1];
        const objects::layout::Vector2i alignmentPosition = { cursor.br[x], cursor.ul[y] };
        toPosition(alignmentPosition, nodeState.pos);

        cursor.br[x] += nodeState.dim[x];
        const int height = cursor.ul[y] + nodeState.dim[y];
        if(height > cursor.br[y])
        {
            cursor.br[y] = height;
        }
    }

    void alignToLineWrap(const objects::layout::Strategy& strategy, objects::layout::NodeItemState& nodeState, objects::layout::NodeItemState& parentNodeState, objects::layout::LineCursor& cursor)
    {
        auto& axes = strategy.axes;
        auto x = axes[0];
        auto y = axes[1];

        if(cursor.br[x] + nodeState.dim[x] > parentNodeState.dim[x])
        {
            cursor.br[x] = 0;
            cursor.ul[y] = cursor.br[y];
        }

        alignToLine(strategy, nodeState, cursor);
    }

    void updateSizeToCursor(const objects::layout::LineCursor& cursor, const objects::layout::NodeItemSetup& nodeSetup, objects::layout::NodeItemState& nodeState)
    {
        for(std::size_t i = 0; i < nodeSetup.stretching.axs.size(); ++i)
        {
            const auto& upstream = nodeSetup.stretching.axs[i].upstream;
            if(upstream && *upstream == objects::layout::Upstream::children)
            {
                if(cursor.br[i] > nodeState.dim[i])
                {
                    nodeState.dim[i] = cursor.br[i];
                }
            }
        }
    }

    void setNodePosition(const objects::layout::NodeSetupList& setupList, objects::layout::NodeStateList& stateList,
        std::size_t nodeIndex, std::size_t parentIndex)
    {
        while(nodeIndex != parentIndex)
        {
            auto& nodeSetup = setupList.nodes[nodeIndex];
            auto& nodeState = stateList.nodes[nodeIndex];
            auto& parentNodeSetup = setupList.nodes[nodeSetup.parent];
            auto& parentNodeState = stateList.nodes[nodeSetup.parent];
            const auto& alignment = nodeSetup.alignment;

            objects::layout::LineCursor& cursor = parentNodeState.cursor;

            if(alignment.originStrategy)
            {
                alignToOrigin(nodeState, cursor);
            }
            else if(alignment.lineStrategy)
            {
                alignToLine(*alignment.lineStrategy, nodeState, cursor);
            }
            else if(alignment.tileStrategy)
            {
                alignToLineWrap(*alignment.tileStrategy, nodeState, parentNodeState, cursor);
            }

            updateSizeToCursor(cursor, parentNodeSetup, parentNodeState);

            auto& xValue = nodeState.position.x;
            auto& yValue = nodeState.position.y;

            switch(alignment.strategy)
            {
            case objects::layout::AlignmentStrategy::origin:
                {
                    cursor.upperLeftCorner.x = 0;
                    cursor.upperLeftCorner.y = 0;
                    cursor.bottomRightCorner.x = nodeState.size.width;
                    cursor.bottomRightCorner.y = nodeState.size.height;

                    xValue = cursor.upperLeftCorner.x;
                    yValue = cursor.upperLeftCorner.y;

                    break;
                }
            case objects::layout::AlignmentStrategy::horizontalLine:
                {
                    xValue = cursor.bottomRightCorner.x;
                    yValue = cursor.upperLeftCorner.y;

                    cursor.bottomRightCorner.x += nodeState.size.width;
                    if(cursor.upperLeftCorner.y + nodeState.size.height > cursor.bottomRightCorner.y)
                    {
                        cursor.bottomRightCorner.y = cursor.upperLeftCorner.y + nodeState.size.height;
                    }

                    break;

                }
            case objects::layout::AlignmentStrategy::verticalLine:
                {
                    xValue = cursor.upperLeftCorner.x;
                    yValue = cursor.bottomRightCorner.y;

                    cursor.bottomRightCorner.y += nodeState.size.height;
                    if(cursor.upperLeftCorner.x + nodeState.size.width > cursor.bottomRightCorner.x)
                    {
                        cursor.bottomRightCorner.x = cursor.upperLeftCorner.x + nodeState.size.width;
                    }

                    break;
                }
            case objects::layout::AlignmentStrategy::horizontalTiles:
                {
                    if(cursor.bottomRightCorner.x + nodeState.size.width > parentNodeState.size.width)
                    {
                        cursor.bottomRightCorner.x = 0;
                        cursor.upperLeftCorner.y = cursor.bottomRightCorner.y;
                    }

                    xValue = cursor.bottomRightCorner.x;
                    yValue = cursor.upperLeftCorner.y;

                    cursor.bottomRightCorner.x += nodeState.size.width;

                    if(nodeState.size.height > cursor.bottomRightCorner.y - cursor.upperLeftCorner.y)
                    {
                        cursor.bottomRightCorner.y = cursor.upperLeftCorner.y + nodeState.size.height;
                    }

                    break;
                }
            case objects::layout::AlignmentStrategy::verticalTiles:
                {
                    if(cursor.bottomRightCorner.y + nodeState.size.height > parentNodeState.size.height)
                    {
                        cursor.bottomRightCorner.y = 0;
                        cursor.upperLeftCorner.x = cursor.bottomRightCorner.x;
                    }

                    yValue = cursor.bottomRightCorner.y;
                    xValue = cursor.upperLeftCorner.x;

                    cursor.bottomRightCorner.y += nodeState.size.height;

                    if(nodeState.size.width > cursor.bottomRightCorner.x - cursor.upperLeftCorner.x)
                    {
                        cursor.bottomRightCorner.x = cursor.upperLeftCorner.x + nodeState.size.width;
                    }

                    break;
                }
            default: break;
            }

            //update parent's size after child node is positioned
            {
                const auto& [widthAxis, heightAxis] = parentNodeSetup.stretching.axes;
                const auto& [upperLeftCorner, bottomRightCorner, ul, br] = parentNodeState.cursor;
                if(widthAxis.value.upstream)
                {
                    if(*widthAxis.value.upstream == objects::layout::Upstream::children)
                    {
                        const int newWidth = bottomRightCorner.x;
                        if(newWidth > parentNodeState.size.width)
                        {
                            parentNodeState.size.width = newWidth;
                        }
                    }
                }

                if(heightAxis.value.upstream)
                {
                    if(heightAxis.value.upstream == objects::layout::Upstream::children)
                    {
                        const int newHeight = bottomRightCorner.y;
                        if(newHeight > parentNodeState.size.height)
                        {
                            parentNodeState.size.height = newHeight;
                        }
                    }
                }
            }

            nodeIndex = parentIndex;
            parentIndex = parentNodeSetup.parent;
            setCursorToNode(setupList, stateList, nodeIndex);
        }
    }

    void calculateSizes(const objects::layout::NodeSetupList& setupList, objects::layout::NodeStateList& stateList, const objects::layout::Variables& variables)
    {
        for(std::size_t i = 0; i < setupList.nodes.size(); ++i)
        {
            auto& nodeSetup = setupList.nodes[i];
            auto& nodeState = stateList.nodes[i];
            auto& parentNodeState = stateList.nodes[nodeSetup.parent];

            const auto& [width, height] = nodeSetup.stretching.axes;
            const auto& [x, y] = nodeSetup.alignment.axes;

            auto& widthValue = nodeState.size.*width.property;
            auto& heightValue = nodeState.size.*height.property;

            widthValue = getSizeValue(width, x, parentNodeState, variables);
            heightValue = getSizeValue(height, y, parentNodeState, variables);

            if(i != nodeSetup.parent)
            {
                setNodePosition(setupList, stateList, i, nodeSetup.parent);
            }
            else
            {
                //set root's position
                nodeState.position.x = getValue(x.value, 0, variables);
                nodeState.position.y = getValue(y.value, 0, variables);
            }
        }
    }

    void calculatePositions(const objects::layout::NodeSetupList& setupList, objects::layout::NodeStateList& stateList, const objects::layout::Variables& variables)
    {
        for(std::size_t i = 0; i < setupList.nodes.size(); ++i)
        {
            const auto& nodeSetup = setupList.nodes[i];
            const auto& nodeState = stateList.nodes[i];

            auto cursor = nodeState.cursor;
            cursor.upperLeftCorner.x = 0;
            cursor.upperLeftCorner.y = 0;
            cursor.bottomRightCorner.x = 0;
            cursor.bottomRightCorner.y = 0;

            for(const auto j : nodeSetup.children)
            {
                auto& childNodeSetup = setupList.nodes[j];
                auto& childNodeState = stateList.nodes[j];

                switch(childNodeSetup.alignment.strategy)
                {
                case objects::layout::AlignmentStrategy::horizontalLine:
                    {
                        childNodeState.position.x = cursor.bottomRightCorner.x;
                        childNodeState.position.y = cursor.upperLeftCorner.y;

                        cursor.bottomRightCorner.x += childNodeState.size.width;
                        if(childNodeState.size.height > cursor.bottomRightCorner.y - cursor.upperLeftCorner.y)
                        {
                            cursor.bottomRightCorner.y = cursor.upperLeftCorner.y + childNodeState.size.height;
                        }

                        break;
                    }
                case objects::layout::AlignmentStrategy::verticalLine:
                    {
                        childNodeState.position.y = cursor.bottomRightCorner.y;
                        childNodeState.position.x = cursor.upperLeftCorner.x;

                        cursor.bottomRightCorner.y += childNodeState.size.height;
                        if(childNodeState.size.height > cursor.bottomRightCorner.x - cursor.upperLeftCorner.x)
                        {
                            cursor.bottomRightCorner.x = cursor.upperLeftCorner.x + childNodeState.size.width;
                        }

                        break;
                    }
                case objects::layout::AlignmentStrategy::relative:
                    {
                        const auto& xAxis = childNodeSetup.alignment.axes.x;
                        const auto& yAxis = childNodeSetup.alignment.axes.y;

                        childNodeState.position.x = getAlignmentValue(xAxis.value, childNodeState.size.width, nodeState.size.width, variables);
                        childNodeState.position.y = getAlignmentValue(yAxis.value, childNodeState.size.height, nodeState.size.height, variables);

                        break;
                    }
                default: break;
                }
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
                    if(parentNodeSetup.children[j] == i)
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
                container->children.resize(nodeSetup.children.size());
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

        //TODO: make the container's tree a flat list
        return buildContainer(setupList, stateList);
    }
}