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
            switch(layout2::Upstream upstreamDefinition = *valueDefinition.upstream)
            {
            case layout2::Upstream::parent:
                {
                    result.upstream = objects::layout::Upstream::parent;
                }
            case layout2::Upstream::children:
                {
                    result.upstream = objects::layout::Upstream::children;
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

    void setupAlignmentAxis(objects::layout::AlignmentAxis& axis, const objects::layout::AlignOrder order, const objects::layout::PositionProperty property, const layout2::Dimension& valueDefinition)
    {
        axis.order = order;
        axis.property = property;
        axis.value = getDimensionValue(valueDefinition);
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
        return getStretching(containerDefinition, true);
    }

    objects::layout::Stretching getRowStretching(const layout2::ContainerDefinition& containerDefinition)
    {
        return getStretching(containerDefinition, true);
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
        return getAlignment(containerDefinition, objects::layout::AlignOrder::origin, objects::layout::AlignOrder::origin);
    }

    objects::layout::NodeSetupList LayoutSetupService::buildSetupList(const layout2::ContainerDefinition& containerDefinition)
    {
        objects::layout::NodeSetupList setupList;

        std::stack<std::tuple<const layout2::ContainerDefinition*, std::size_t, objects::layout::Alignment, objects::layout::Stretching>> stack;
        stack.emplace(&containerDefinition, 0, getSlideAlignment(containerDefinition), getSlideStretching(containerDefinition));

        std::size_t i {};
        while(!stack.empty())
        {
            auto [definition, parentIndex, alignment, stretching] = stack.top();
            stack.pop();

            setupList.nodes.emplace_back(definition->name, parentIndex, std::vector<std::size_t>{}, alignment, stretching);
            if(parentIndex != i)
            {
                objects::layout::NodeItemSetup& parendNode = setupList.nodes[parentIndex];
                parendNode.children.emplace_back(i);
            }

            for(const auto& childDefinition : definition->floating)
            {
                stack.emplace(&childDefinition, i, getFloatingAlignment(containerDefinition), getFloatingStretching(childDefinition));
            }

            for(const auto& childDefinition : definition->columns)
            {
                stack.emplace(&childDefinition, i, getColumnAlignment(containerDefinition), getColumnStretching(childDefinition));
            }

            for(const auto& childDefinition : definition->rows)
            {
                stack.emplace(&childDefinition, i, getRowAlignment(containerDefinition), getRowStretching(childDefinition));
            }

            for(const auto& childDefinition : definition->tileRow)
            {
                stack.emplace(&childDefinition, i, getTileRowAlignment(containerDefinition), getTileStretching(childDefinition));
            }

            for(const auto& childDefinition : definition->tileColumn)
            {
                stack.emplace(&childDefinition, i, getTileColumnAlignment(containerDefinition), getTileStretching(childDefinition));
            }

            for(const auto& childDefinition : definition->slides)
            {
                stack.emplace(&childDefinition, i, getSlideAlignment(containerDefinition), getSlideStretching(childDefinition));
            }

            ++i;
        }

        return setupList;
    }
}
