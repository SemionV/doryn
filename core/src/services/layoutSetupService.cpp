#include <dory/core/services/layoutSetupService.h>
#include <stack>

namespace dory::core::services
{
    using namespace resources;
    using namespace resources::scene;
    using namespace resources::scene::configuration;

    void getDimensionValue(const layout2::Dimension& valueDefinition, objects::layout::DimensionValue& result)
    {
        if(valueDefinition.pixels)
        {
            result.pixels = *valueDefinition.pixels;
        }
        else if(valueDefinition.percents)
        {
            result.percents = *valueDefinition.percents;
        }
        else if(valueDefinition.variable)
        {
            result.variable = *valueDefinition.variable;
        }
    }

    objects::layout::PositionValue getPositionValue(const layout2::DimensionPoint& valueDefinition)
    {
        objects::layout::PositionValue result;
        if(valueDefinition.align)
        {
            if(valueDefinition.align.value() == layout2::Align::center)
            {
                result.order = objects::layout::AlignOrder::center;
            }
        }

        getDimensionValue(valueDefinition, result);

        return result;
    }

    objects::layout::SizeValue getSizeValue(const layout2::DimensionSegment& valueDefinition)
    {
        objects::layout::SizeValue result {};

        if(valueDefinition.upstream)
        {
            switch(*valueDefinition.upstream)
            {
            case layout2::Upstream::parent:
                {
                    result.upstream = objects::layout::Upstream::parent;
                    break;
                }
            case layout2::Upstream::fill:
                {
                    result.upstream = objects::layout::Upstream::fill;
                    break;
                }
            case layout2::Upstream::children:
                {
                    result.upstream = objects::layout::Upstream::children;
                    break;
                }
            default: result.upstream = objects::layout::Upstream::parent;
            }
        }

        getDimensionValue(valueDefinition, result);

        return result;
    }

    void setupStretchingAxis(objects::layout::StretchingAxis& axis, const objects::layout::SizeProperty property, const layout2::DimensionSegment& valueDefinition)
    {
        axis.property = property;
        axis.value = getSizeValue(valueDefinition);
    }

    void setupAlignmentAxis(objects::layout::AlignmentAxis& axis, const objects::layout::PositionProperty property)
    {
        axis.property = property;
    }

    void setupAlignmentAxis(objects::layout::AlignmentAxis& axis, const objects::layout::PositionProperty property, const layout2::DimensionPoint& valueDefinition)
    {
        setupAlignmentAxis(axis, property);
        axis.value = getPositionValue(valueDefinition);
    }

    void setupAlignmentAxis(objects::layout::AlignmentAxis& axis, const objects::layout::PositionProperty property, const int value)
    {
        setupAlignmentAxis(axis, property);
        axis.value.pixels = value;
    }

    objects::layout::Stretching getStretching(const layout2::ContainerDefinition& containerDefinition)
    {
        objects::layout::Stretching stretching;
        setupStretchingAxis(stretching.axes.width, &objects::layout::Size::width, containerDefinition.width);
        setupStretchingAxis(stretching.axes.height, &objects::layout::Size::height, containerDefinition.height);
        return stretching;
    }

    objects::layout::Stretching getColumnStretching(const layout2::ContainerDefinition& containerDefinition)
    {
        objects::layout::Stretching stretching;
        setupStretchingAxis(stretching.axes.width, &objects::layout::Size::width, containerDefinition.width);
        auto& heightAxis = stretching.axes.height;
        heightAxis.property = &objects::layout::Size::height;
        heightAxis.value.upstream = objects::layout::Upstream::parent;

        return stretching;
    }

    objects::layout::Stretching getRowStretching(const layout2::ContainerDefinition& containerDefinition)
    {
        objects::layout::Stretching stretching;
        setupStretchingAxis(stretching.axes.height, &objects::layout::Size::height, containerDefinition.height);
        auto& widthAxis = stretching.axes.width;
        widthAxis.property = &objects::layout::Size::width;
        widthAxis.value.upstream = objects::layout::Upstream::parent;

        return stretching;
    }

    objects::layout::Alignment getAlignment(const layout2::ContainerDefinition& containerDefinition)
    {
        objects::layout::Alignment alignment;
        setupAlignmentAxis(alignment.axes.x, &objects::layout::Position::x, containerDefinition.x);
        setupAlignmentAxis(alignment.axes.y, &objects::layout::Position::y, containerDefinition.y);
        return alignment;
    }

    objects::layout::Alignment getColumnAlignment(const layout2::ContainerDefinition& containerDefinition)
    {
        auto alignment = getAlignment(containerDefinition);
        alignment.strategy = objects::layout::AlignmentStrategy::horizontalLine;
        return alignment;
    }

    objects::layout::Alignment getRowAlignment(const layout2::ContainerDefinition& containerDefinition)
    {
        auto alignment = getAlignment(containerDefinition);
        alignment.strategy = objects::layout::AlignmentStrategy::verticalLine;
        return alignment;
    }

    objects::layout::Alignment getTileRowAlignment(const layout2::ContainerDefinition& containerDefinition)
    {
        auto alignment = getAlignment(containerDefinition);
        alignment.strategy = objects::layout::AlignmentStrategy::horizontalTiles;
        return alignment;
    }

    objects::layout::Alignment getTileColumnAlignment(const layout2::ContainerDefinition& containerDefinition)
    {
        auto alignment = getAlignment(containerDefinition);
        alignment.strategy = objects::layout::AlignmentStrategy::verticalTiles;
        return alignment;
    }

    void setupFloatingAlignmentAxis(objects::layout::AlignmentAxis& axis, const objects::layout::PositionProperty positionProperty, const layout2::DimensionPoint& dimension)
    {
        if(dimension.align == layout2::Align::center)
        {
            axis.value.order = objects::layout::AlignOrder::center;
        }

        setupAlignmentAxis(axis, positionProperty, dimension);
    }

    objects::layout::Alignment getFloatingAlignment(const layout2::ContainerDefinition& containerDefinition)
    {
        objects::layout::Alignment alignment;
        setupFloatingAlignmentAxis(alignment.axes.x, &objects::layout::Position::x, containerDefinition.x);
        setupFloatingAlignmentAxis(alignment.axes.y, &objects::layout::Position::y, containerDefinition.y);

        alignment.strategy = objects::layout::AlignmentStrategy::relative;
        return alignment;
    }

    objects::layout::Alignment getSlideAlignment(const layout2::ContainerDefinition& containerDefinition)
    {
        objects::layout::Alignment alignment;
        setupAlignmentAxis(alignment.axes.x, &objects::layout::Position::x, 0);
        setupAlignmentAxis(alignment.axes.y, &objects::layout::Position::y, 0);

        alignment.strategy = objects::layout::AlignmentStrategy::origin;
        return alignment;
    }

    struct StackNodeEntry
    {
        const layout2::ContainerDefinition* definition {};
        std::size_t parentIndex {};
        std::size_t index {};
        objects::layout::Alignment alignment {};
        objects::layout::Alignment2 alignment2 {};
        objects::layout::Stretching stretching {};
    };

    void addChildDefinitions(std::vector<StackNodeEntry>& children, std::size_t parentIndex,
        const std::vector<layout2::ContainerDefinition>& definitions, const auto& getAlignment, const auto& getStretching)
    {
        std::vector<StackNodeEntry> flexibleChildren {};

        for(std::size_t i = 0; i < definitions.size(); ++i)
        {
            const auto& definition = definitions[i];
            if(definition.width.upstream == layout2::Upstream::fill ||
                definition.height.upstream == layout2::Upstream::fill)
            {
                flexibleChildren.emplace_back(&definition, parentIndex, i, getAlignment(definition), objects::layout::Alignment2{}, getStretching(definition));
            }
            else
            {
                children.emplace_back(&definition, parentIndex, i, getAlignment(definition),  objects::layout::Alignment2{}, getStretching(definition));
            }
        }

        for(auto& entry : flexibleChildren)
        {
            children.emplace_back(std::move(entry));
        }
    }

    objects::layout::NodeSetupList LayoutSetupService::buildSetupList(const layout2::ContainerDefinition& containerDefinition)
    {
        objects::layout::NodeSetupList setupList;

        std::stack<StackNodeEntry> stack;
        stack.emplace(&containerDefinition, 0, 0, getSlideAlignment(containerDefinition), objects::layout::Alignment2{}, getStretching(containerDefinition));

        auto columnAlignment = [](const auto& def){return getColumnAlignment(def);};
        auto columnStretching = [](const auto& def){return getColumnStretching(def);};
        auto rowAlignment = [](const auto& def){return getRowAlignment(def);};
        auto rowStretching = [](const auto& def){return getRowStretching(def);};
        auto tileRowAlignment = [](const auto& def){return getTileRowAlignment(def);};
        auto tileColumnAlignment = [](const auto& def){return getTileColumnAlignment(def);};
        auto slideAlignment = [](const auto& def){return getSlideAlignment(def);};
        auto floatingAlignment = [](const auto& def){return getFloatingAlignment(def);};
        auto generalStretching = [](const auto& def){return getStretching(def);};

        std::size_t i {};
        while(!stack.empty())
        {
            auto [definition, parentIndex, index, alignment, alignment2, stretching] = stack.top();
            stack.pop();

            objects::layout::NodeItemSetup& node = setupList.nodes.emplace_back(definition->name, parentIndex, std::vector<std::size_t>{}, alignment, alignment2, stretching);
            if(parentIndex != i)
            {
                objects::layout::NodeItemSetup& parendNode = setupList.nodes[parentIndex];
                parendNode.children[index] = i;
            }

            std::vector<StackNodeEntry> children;

            addChildDefinitions(children, i, definition->columns, columnAlignment, columnStretching);
            addChildDefinitions(children, i, definition->rows, rowAlignment, rowStretching);
            addChildDefinitions(children, i, definition->tileRow, tileRowAlignment, generalStretching);
            addChildDefinitions(children, i, definition->tileColumn, tileColumnAlignment, generalStretching);
            addChildDefinitions(children, i, definition->slides, slideAlignment, generalStretching);
            addChildDefinitions(children, i, definition->floating, floatingAlignment, generalStretching);

            node.children.resize(children.size());

            for(std::size_t j = children.size(); j > 0; --j)
            {
                stack.emplace(children[j - 1]);
            }

            ++i;
        }

        return setupList;
    }
}
