#include <dory/core/services/layoutSetupService.h>
#include <stack>

namespace dory::core::services
{
    using namespace resources;
    using namespace resources::scene;
    using namespace resources::scene::configuration;

    objects::layout::DimensionValue getDimensionValue(const layout2::Dimension& valueDefinition)
    {
        objects::layout::DimensionValue result;

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

        return result;
    }

    objects::layout::SizeValue getSizeValue(const layout2::DimensionSegment& valueDefinition)
    {
        objects::layout::SizeValue result { getDimensionValue(valueDefinition), objects::layout::Upstream::self };

        if(valueDefinition.upstream)
        {
            switch(*valueDefinition.upstream)
            {
            case layout2::Upstream::parent:
                {
                    result.upstream = objects::layout::Upstream::parent;
                    break;
                }
            case layout2::Upstream::children:
                {
                    result.upstream = objects::layout::Upstream::children;
                    break;
                }
            default: result.upstream = objects::layout::Upstream::self;
            }
        }

        return result;
    }

    void setupStretchingAxis(objects::layout::StretchingAxis& axis, const objects::layout::SizeProperty property, const layout2::DimensionSegment& valueDefinition, const bool valueTransparent)
    {
        axis.property = property;
        axis.valuePropagation = valueTransparent;
        axis.value = getSizeValue(valueDefinition);
    }

    void setupAlignmentAxis(objects::layout::AlignmentAxis& axis, const objects::layout::AlignOrder order, const objects::layout::PositionProperty property)
    {
        axis.order = order;
        axis.property = property;
    }

    void setupAlignmentAxis(objects::layout::AlignmentAxis& axis, const objects::layout::AlignOrder order, const objects::layout::PositionProperty property, const layout2::Dimension& valueDefinition)
    {
        setupAlignmentAxis(axis, order, property);
        axis.value = getDimensionValue(valueDefinition);
    }

    void setupAlignmentAxis(objects::layout::AlignmentAxis& axis, const objects::layout::AlignOrder order, const objects::layout::PositionProperty property, const int value)
    {
        setupAlignmentAxis(axis, order, property);
        axis.value.pixels = value;
    }

    objects::layout::Stretching getStretching(const layout2::ContainerDefinition& containerDefinition, const bool valueTransparent)
    {
        objects::layout::Stretching stretching;
        setupStretchingAxis(stretching.axes.width, &objects::layout::Size::width, containerDefinition.width, valueTransparent);
        setupStretchingAxis(stretching.axes.height, &objects::layout::Size::height, containerDefinition.height, valueTransparent);
        return stretching;
    }

    objects::layout::Stretching getColumnStretching(const layout2::ContainerDefinition& containerDefinition)
    {
        objects::layout::Stretching stretching;
        setupStretchingAxis(stretching.axes.width, &objects::layout::Size::width, containerDefinition.width, true);
        auto& heightAxis = stretching.axes.height;
        heightAxis.property = &objects::layout::Size::height;
        heightAxis.valuePropagation = false;
        heightAxis.value.upstream = objects::layout::Upstream::parent;

        return stretching;
    }

    objects::layout::Stretching getRowStretching(const layout2::ContainerDefinition& containerDefinition)
    {
        objects::layout::Stretching stretching;
        setupStretchingAxis(stretching.axes.height, &objects::layout::Size::height, containerDefinition.height, true);
        auto& widthAxis = stretching.axes.width;
        widthAxis.property = &objects::layout::Size::width;
        widthAxis.valuePropagation = false;
        widthAxis.value.upstream = objects::layout::Upstream::parent;

        return stretching;
    }

    objects::layout::Stretching getTileStretching(const layout2::ContainerDefinition& containerDefinition)
    {
        return getStretching(containerDefinition, false);
    }

    objects::layout::Stretching getFloatingStretching(const layout2::ContainerDefinition& containerDefinition)
    {
        return getStretching(containerDefinition, false);
    }

    objects::layout::Stretching getSlideStretching(const layout2::ContainerDefinition& containerDefinition)
    {
        return getStretching(containerDefinition, false);
    }

    objects::layout::Alignment getAlignment(const layout2::ContainerDefinition& containerDefinition, objects::layout::AlignOrder xOrder, objects::layout::AlignOrder yOrder)
    {
        objects::layout::Alignment alignment;
        setupAlignmentAxis(alignment.axes.x, xOrder, &objects::layout::Position::x, containerDefinition.x);
        setupAlignmentAxis(alignment.axes.y, yOrder, &objects::layout::Position::y, containerDefinition.y);
        return alignment;
    }

    objects::layout::Alignment getColumnAlignment(const layout2::ContainerDefinition& containerDefinition)
    {
        return getAlignment(containerDefinition, objects::layout::AlignOrder::line, objects::layout::AlignOrder::origin);
    }

    objects::layout::Alignment getRowAlignment(const layout2::ContainerDefinition& containerDefinition)
    {
        return getAlignment(containerDefinition, objects::layout::AlignOrder::origin, objects::layout::AlignOrder::line);
    }

    objects::layout::Alignment getTileRowAlignment(const layout2::ContainerDefinition& containerDefinition)
    {
        return getAlignment(containerDefinition, objects::layout::AlignOrder::line, objects::layout::AlignOrder::wrap);
    }

    objects::layout::Alignment getTileColumnAlignment(const layout2::ContainerDefinition& containerDefinition)
    {
        return getAlignment(containerDefinition, objects::layout::AlignOrder::wrap, objects::layout::AlignOrder::line);
    }

    objects::layout::Alignment getFloatingAlignment(const layout2::ContainerDefinition& containerDefinition)
    {
        return getAlignment(containerDefinition, objects::layout::AlignOrder::relative, objects::layout::AlignOrder::relative);
    }

    objects::layout::Alignment getSlideAlignment(const layout2::ContainerDefinition& containerDefinition)
    {
        objects::layout::Alignment alignment;
        setupAlignmentAxis(alignment.axes.x, objects::layout::AlignOrder::origin, &objects::layout::Position::x, 0);
        setupAlignmentAxis(alignment.axes.y, objects::layout::AlignOrder::origin, &objects::layout::Position::y, 0);
        return alignment;
    }

    struct StackNodeEntry
    {
        const layout2::ContainerDefinition* definition {};
        std::size_t parentIndex {};
        std::size_t index {};
        objects::layout::Alignment alignment {};
        objects::layout::Stretching stretching {};
    };

    void addChildDefinitions(std::vector<StackNodeEntry>& children, std::size_t parentIndex,
        const std::vector<layout2::ContainerDefinition>& definitions, const auto& getAlignment, const auto& getStretching)
    {
        std::vector<StackNodeEntry> flexibleChildren {};

        for(std::size_t i = 0; i < definitions.size(); ++i)
        {
            const auto& definition = definitions[i];
            if(definition.width.upstream == layout2::Upstream::parent ||
                definition.height.upstream == layout2::Upstream::parent)
            {
                flexibleChildren.emplace_back(&definition, parentIndex, i, getAlignment(definition), getStretching(definition));
            }
            else
            {
                children.emplace_back(&definition, parentIndex, i, getAlignment(definition), getStretching(definition));
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
        stack.emplace(&containerDefinition, 0, 0, getSlideAlignment(containerDefinition), getSlideStretching(containerDefinition));

        auto columnAlignment = [](const auto& def){return getColumnAlignment(def);};
        auto columnStretching = [](const auto& def){return getColumnStretching(def);};
        auto rowAlignment = [](const auto& def){return getRowAlignment(def);};
        auto rowStretching = [](const auto& def){return getRowStretching(def);};
        auto tileRowAlignment = [](const auto& def){return getTileRowAlignment(def);};
        auto tileColumnAlignment = [](const auto& def){return getTileColumnAlignment(def);};
        auto tileStretching = [](const auto& def){return getTileStretching(def);};
        auto slideAlignment = [](const auto& def){return getSlideAlignment(def);};
        auto slideStretching = [](const auto& def){return getSlideStretching(def);};
        auto floatingAlignment = [](const auto& def){return getFloatingAlignment(def);};
        auto floatingStretching = [](const auto& def){return getFloatingStretching(def);};

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
            addChildDefinitions(children, i, definition->tileRow, tileRowAlignment, tileStretching);
            addChildDefinitions(children, i, definition->tileColumn, tileColumnAlignment, tileStretching);
            addChildDefinitions(children, i, definition->slides, slideAlignment, slideStretching);
            addChildDefinitions(children, i, definition->floating, floatingAlignment, floatingStretching);

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
