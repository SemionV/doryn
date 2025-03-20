#include <dory/core/services/layoutService.h>
#include <cmath>

namespace dory::core::services
{
    using namespace resources;
    using namespace resources::scene;
    using namespace resources::scene::configuration;
    using namespace resources::entities::layout;

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

    int getSizeValue(const objects::layout::SizeValue& value, const std::size_t axis, const objects::layout::NodeItemState& parentState,
        const objects::layout::Variables& variables)
    {
        int result {};

        if(value.upstream)
        {
            if(*value.upstream == objects::layout::Upstream::fill)
            {
                result = parentState.dim[axis] - parentState.cursor.br[axis];
            }
            else if(*value.upstream == objects::layout::Upstream::parent)
            {
                result = parentState.dim[axis];
            }
        }
        else
        {
            result = getValue(value, parentState.dim[axis], variables);
        }

        return result;
    }

    objects::layout::NodeStateList buildNodeList(const objects::layout::NodeSetupList& setupList)
    {
        objects::layout::NodeStateList list;
        list.nodes.resize(setupList.nodes.size());
        return list;
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

    void updateSizeToCursor(const objects::layout::LineCursor& cursor, const objects::layout::NodeItemSetup& nodeSetup, objects::layout::NodeItemState& nodeState)
    {
        for(std::size_t i = 0; i < nodeSetup.stretching.axes.size(); ++i)
        {
            const auto& upstream = nodeSetup.stretching.axes[i].upstream;
            if(upstream && *upstream == objects::layout::Upstream::children)
            {
                if(cursor.br[i] > nodeState.dim[i])
                {
                    nodeState.dim[i] = cursor.br[i];
                }
            }
        }
    }

    void align(const objects::layout::Alignment& strategy, const objects::layout::NodeItemState& parentNodeState,
        objects::layout::NodeItemState& nodeState, objects::layout::LineCursor& cursor, const objects::layout::Variables& variables)
    {
        if(strategy.fixedPosition)
        {
            //fixed
            auto& positionValues = *strategy.fixedPosition;
            for(std::size_t i = 0; i < strategy.axes.size(); ++i)
            {
                nodeState.pos[i] = getAlignmentValue(positionValues[i], nodeState.dim[i], parentNodeState.dim[i], variables);
            }
        }
        else
        {
            //line
            auto& axes = strategy.axes;
            const auto x = axes[objects::layout::Axes::x];
            const auto y = axes[objects::layout::Axes::y];

            if(strategy.lineWrap && cursor.br[x] + nodeState.dim[x] > parentNodeState.dim[x])
            {
                cursor.br[x] = 0;
                cursor.ul[y] = cursor.br[y];
            }

            objects::layout::Vector2i alignmentPosition = { cursor.br[x], cursor.ul[y] };
            alignmentPosition[x] = cursor.br[x];
            alignmentPosition[y] = cursor.ul[y];
            toPosition(alignmentPosition, nodeState.pos);

            cursor.br[x] += nodeState.dim[x];
            const int height = cursor.ul[y] + nodeState.dim[y];
            if(height > cursor.br[y])
            {
                cursor.br[y] = height;
            }
        }
    }

    void updateBranch(const objects::layout::NodeSetupList& setupList, objects::layout::NodeStateList& stateList,
        std::size_t nodeIndex, std::size_t parentIndex, const objects::layout::Variables& variables)
    {
        do
        {
            auto& nodeSetup = setupList.nodes[nodeIndex];
            auto& nodeState = stateList.nodes[nodeIndex];
            auto& parentNodeSetup = setupList.nodes[nodeSetup.parent];
            auto& parentNodeState = stateList.nodes[nodeSetup.parent];
            const auto& alignment = nodeSetup.alignment;
            objects::layout::LineCursor& cursor = parentNodeState.cursor;

            if(!alignment.floating)
            {
                align(alignment, parentNodeState, nodeState, cursor, variables);
            }

            updateSizeToCursor(cursor, parentNodeSetup, parentNodeState);
            nodeIndex = parentIndex;
            parentIndex = parentNodeSetup.parent;
        }
        while(nodeIndex != parentIndex);
    }

    void calculateSizes(const objects::layout::NodeSetupList& setupList, objects::layout::NodeStateList& stateList, const objects::layout::Variables& variables)
    {
        for(std::size_t i = 0; i < setupList.nodes.size(); ++i)
        {
            auto& nodeSetup = setupList.nodes[i];
            auto& nodeState = stateList.nodes[i];
            auto& parentNodeState = stateList.nodes[nodeSetup.parent];

            for(std::size_t a = 0; a < nodeState.dim.size(); ++a)
            {
                nodeState.dim[a] = getSizeValue(nodeSetup.stretching.axes[a], a, parentNodeState, variables);
            }

            updateBranch(setupList, stateList, i, nodeSetup.parent, variables);
        }
    }

    void calculatePositions(const objects::layout::NodeSetupList& setupList, objects::layout::NodeStateList& stateList, const objects::layout::Variables& variables)
    {
        for(std::size_t i = 0; i < setupList.nodes.size(); ++i)
        {
            const auto& nodeSetup = setupList.nodes[i];
            auto& nodeState = stateList.nodes[i];

            auto& cursor = nodeState.cursor;
            toOrigin(cursor);

            for(const auto j : nodeSetup.children)
            {
                auto& childNodeSetup = setupList.nodes[j];
                auto& childNodeState = stateList.nodes[j];

                align(childNodeSetup.alignment, nodeState, childNodeState, nodeState.cursor, variables);
            }
        }
    }

    void setupContainer(const objects::layout::NodeItemSetup& nodeSetup, const objects::layout::NodeItemState& nodeState, Container& container)
    {
        container.children.resize(nodeSetup.children.size());
        container.name = nodeSetup.name;
        container.position.x = nodeState.pos[objects::layout::Axes::x];
        container.position.y = nodeState.pos[objects::layout::Axes::y];
        container.size.width = nodeState.dim[objects::layout::Axes::x];
        container.size.height = nodeState.dim[objects::layout::Axes::y];
    }

    std::unique_ptr<Container> buildContainer(const objects::layout::NodeSetupList& setupList, const objects::layout::NodeStateList& stateList)
    {
        std::unique_ptr<Container> rootContainer {};
        std::vector<Container*> lookupTable(stateList.nodes.size());

        for(std::size_t i = 0; i < stateList.nodes.size(); ++i)
        {
            const auto& nodeState = stateList.nodes[i];
            const auto& nodeSetup = setupList.nodes[i];

            Container* container {};

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
                rootContainer = std::make_unique<Container>();
                lookupTable[i] = container = rootContainer.get();
            }

            if(container)
            {
                setupContainer(nodeSetup, nodeState, *container);
            }
        }

        return rootContainer;
    }

    objects::layout::NodeStateList calculateLayout(const objects::layout::NodeSetupList& setupList, const objects::layout::Variables& variables)
    {
        objects::layout::NodeStateList stateList = buildNodeList(setupList);
        calculateSizes(setupList, stateList, variables);
        calculatePositions(setupList, stateList, variables);

        return stateList;
    }

    //Explanation: https://youtu.be/JZB5eRQNWOA
    void buildContainers(const objects::layout::NodeSetupList& setupList, const objects::layout::NodeStateList& stateList, Layout& layout)
    {
        layout.containers.clear();
        layout.containers.reserve(setupList.nodes.size());

        if(!setupList.nodes.empty())
        {
            std::stack<std::tuple<std::size_t, std::size_t>> stack {};
            std::vector<std::size_t> lookupTable;
            lookupTable.resize(setupList.nodes.size());

            std::size_t j {};

            stack.emplace(0, 0);
            while(stack.empty())
            {
                auto [i, parentIndex] = stack.top();
                stack.pop();

                lookupTable[i] = j;

                Container& container = layout.containers[j];
                const objects::layout::NodeItemSetup& nodeSetup = setupList.nodes[i];
                const objects::layout::NodeItemState& nodeState = stateList.nodes[i];
                container.name = nodeSetup.name;
                container.position.x = nodeState.pos[objects::layout::Axes::x];
                container.position.y = nodeState.pos[objects::layout::Axes::y];
                container.size.width = nodeState.dim[objects::layout::Axes::x];
                container.size.height = nodeState.dim[objects::layout::Axes::y];
                container.childIndices.reserve(nodeSetup.children.size());
                container.parent = lookupTable[nodeSetup.parent];

                if(container.parent != j)
                {
                    Container& parentContainer = layout.containers[container.parent];
                    parentContainer.childIndices.emplace_back(j);
                }

                for(std::size_t k = nodeSetup.children.size(); k > 0; --k)
                {
                    stack.emplace(k - 1, i);
                }
            }
        }
    }

    std::unique_ptr<Container> LayoutService::calculate(const objects::layout::NodeSetupList& setupList, const objects::layout::Variables& variables)
    {
        objects::layout::NodeStateList stateList = buildNodeList(setupList);
        calculateSizes(setupList, stateList, variables);
        calculatePositions(setupList, stateList, variables);

        //TODO: make the container's tree a flat list
        return buildContainer(setupList, stateList);
    }

    void LayoutService::buildLayout(const objects::layout::NodeSetupList& setupList, const objects::layout::Variables& variables, Layout& layout)
    {
        const auto stateList = calculateLayout(setupList, variables);
        buildContainers(setupList, stateList, layout);
    }
}
