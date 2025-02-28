#include <dory/core/services/layoutService2.h>
#include <cmath>

namespace dory::core::services
{
    using namespace resources;
    using namespace resources::scene;
    using namespace resources::scene::configuration;
    using namespace resources::scene::configuration;

    objects::layout::Container LayoutService2::calculate(const layout::ContainerDefinition& layoutDefinition)
    {
        objects::layout::Container root;

        return root;
    }

    int getDimensionValue(const objects::layout::StretchingAxis& axis, const objects::layout::Size& parentSize,
        const objects::layout::Size& parentContentSize, const objects::layout::Variables& variables)
    {
        int result {};

        const auto value = axis.value;
        if(value.upstream == objects::layout::Upstream::self)
        {
            if(value.pixels)
            {
                result = *value.pixels;
            }
            else if(value.percents)
            {
                result = static_cast<int>(std::round(*value.percents * 0.01f * static_cast<float>(parentSize.*axis.property)));
            }
            else if(value.variable)
            {
                //TODO: read value from a variable
            }
        }
        else if(value.upstream == objects::layout::Upstream::parent)
        {
            result = parentSize.*axis.property - parentContentSize.*axis.property;
        }

        return result;
    }

    void propagateValue(std::size_t nodeIndex, std::size_t parentIndex, const objects::layout::StretchingAxisProperty axisProperty, objects::layout::NodeList& list)
    {
        const auto& node = list.nodes[nodeIndex];
        const objects::layout::StretchingAxis& axis = node.stretching.axes.*axisProperty;

        if(axis.valuePropagation)
        {
            const int value = node.size.*axis.property;

            while(nodeIndex != parentIndex)
            {
                auto& parentNode = list.nodes[parentIndex];
                const objects::layout::StretchingAxis& parentAxis = parentNode.stretching.axes.*axisProperty;

                parentNode.contentSize.*parentAxis.property += value;

                if(parentAxis.value.upstream == objects::layout::Upstream::children)
                {
                    parentNode.size.*parentAxis.property += value;
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
                parentIndex = parentNode.parent;
            }
        }
    }

    void calculateSizes(objects::layout::NodeList& list, const objects::layout::Variables& variables)
    {
        for(std::size_t i = 0; i < list.nodes.size(); ++i)
        {
            auto& node = list.nodes[i];
            auto& parentNode = list.nodes[node.parent];

            const auto& stretching = node.stretching;

            auto& width = node.size.*stretching.axes.width.property;
            auto& height = node.size.*stretching.axes.height.property;

            width = getDimensionValue(stretching.axes.width, parentNode.size, parentNode.contentSize, variables);
            height = getDimensionValue(stretching.axes.height, parentNode.size, parentNode.contentSize, variables);

            propagateValue(i, node.parent, &objects::layout::StretchingAxes::width, list);
            propagateValue(i, node.parent, &objects::layout::StretchingAxes::height, list);
        }
    }
}

