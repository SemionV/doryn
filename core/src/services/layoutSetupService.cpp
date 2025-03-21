#include <dory/core/services/layoutSetupService.h>
#include <stack>

namespace dory::core::services
{
    using namespace resources;
    using namespace resources::scene;
    using namespace resources::scene::configuration;

    void getDimensionValue(const layout::Dimension& valueDefinition, objects::layout::DimensionValue& result)
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

    objects::layout::PositionValue getPositionValue(const layout::DimensionPoint& valueDefinition)
    {
        objects::layout::PositionValue result;
        if(valueDefinition.align)
        {
            if(valueDefinition.align.value() == layout::Align::center)
            {
                result.order = objects::layout::AlignOrder::center;
            }
        }

        getDimensionValue(valueDefinition, result);

        return result;
    }

    objects::layout::SizeValue getSizeValue(const layout::DimensionSegment& valueDefinition)
    {
        objects::layout::SizeValue result {};

        if(valueDefinition.upstream)
        {
            switch(*valueDefinition.upstream)
            {
            case layout::Upstream::parent:
                {
                    result.upstream = objects::layout::Upstream::parent;
                    break;
                }
            case layout::Upstream::fill:
                {
                    result.upstream = objects::layout::Upstream::fill;
                    break;
                }
            case layout::Upstream::children:
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

    objects::layout::Stretching getStretching(const layout::ContainerDefinition& containerDefinition)
    {
        objects::layout::Stretching stretching;
        stretching.axes[objects::layout::Axes::x] = getSizeValue(containerDefinition.width);
        stretching.axes[objects::layout::Axes::y] = getSizeValue(containerDefinition.height);
        return stretching;
    }

    objects::layout::Stretching getColumnStretching(const std::size_t fixedAxis, const layout::ContainerDefinition& containerDefinition)
    {
        objects::layout::Stretching stretching = getStretching(containerDefinition);
        stretching.axes[fixedAxis].upstream = objects::layout::Upstream::parent;
        return stretching;
    }

    objects::layout::Alignment getAlignment(const bool lineWrap, const std::array<std::size_t, 2>& axes)
    {
        objects::layout::Alignment alignment;
        alignment.lineWrap = lineWrap;
        alignment.floating = false;
        alignment.axes = axes;
        return alignment;
    }

    objects::layout::Alignment getAlignment(const bool floating, const std::array<std::size_t, 2>& axes, const layout::ContainerDefinition& containerDefinition)
    {
        objects::layout::Alignment alignment;
        alignment.lineWrap = false;
        alignment.floating = floating;
        alignment.axes = axes;

        std::array<objects::layout::PositionValue, 2> values {};
        values[axes[objects::layout::Axes::x]] = getPositionValue(containerDefinition.x);
        values[axes[objects::layout::Axes::y]] = getPositionValue(containerDefinition.y);
        alignment.fixedPosition = values;

        return alignment;
    }

    struct StackNodeEntry
    {
        const layout::ContainerDefinition* definition {};
        std::size_t parentIndex {};
        std::size_t index {};
        objects::layout::Alignment alignment {};
        objects::layout::Stretching stretching {};
    };

    void addChildDefinitions(std::vector<StackNodeEntry>& children, const std::size_t parentIndex,
        const std::vector<layout::ContainerDefinition>& definitions, const auto& getAlignment, const auto& getStretching)
    {
        std::vector<StackNodeEntry> flexibleChildren {};

        const std::size_t offset = children.size();
        for(std::size_t i = 0; i < definitions.size(); ++i)
        {
            const auto& definition = definitions[i];
            if(definition.width.upstream == layout::Upstream::fill ||
                definition.height.upstream == layout::Upstream::fill)
            {
                flexibleChildren.emplace_back(&definition, parentIndex, i, getAlignment(definition), getStretching(definition));
            }
            else
            {
                children.emplace_back(&definition, parentIndex, i + offset, getAlignment(definition), getStretching(definition));
            }
        }

        for(auto& entry : flexibleChildren)
        {
            children.emplace_back(std::move(entry));
        }
    }

    objects::layout::NodeSetupList LayoutSetupService::buildSetupList(const layout::ContainerDefinition& containerDefinition)
    {
        objects::layout::NodeSetupList setupList;

        std::stack<StackNodeEntry> stack;
        stack.emplace(&containerDefinition, 0, 0, getAlignment(false, objects::layout::Axes::xy, containerDefinition), getStretching(containerDefinition));

        auto columnAlignment = [](const auto& def){return getAlignment(false, objects::layout::Axes::xy);};
        auto columnStretching = [](const auto& def){return getColumnStretching(objects::layout::Axes::y, def);};
        auto rowAlignment = [](const auto& def){return getAlignment(false, objects::layout::Axes::yx);};
        auto rowStretching = [](const auto& def){return getColumnStretching(objects::layout::Axes::x, def);};
        auto tileRowAlignment = [](const auto& def){return getAlignment(true, objects::layout::Axes::xy);};
        auto tileColumnAlignment = [](const auto& def){return getAlignment(true, objects::layout::Axes::yx);};
        auto slideAlignment = [](const auto& def){return getAlignment(false, objects::layout::Axes::xy, def);};
        auto floatingAlignment = [](const auto& def){return getAlignment(true, objects::layout::Axes::xy, def);};
        auto generalStretching = [](const auto& def){return getStretching(def);};

        std::size_t i {};
        while(!stack.empty())
        {
            auto [definition, parentIndex, index, alignment, stretching] = stack.top();
            stack.pop();

            objects::layout::NodeItemSetup& node = setupList.nodes.emplace_back(definition->name, parentIndex, std::vector<std::size_t>{}, alignment, stretching);
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
