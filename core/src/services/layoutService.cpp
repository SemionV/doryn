#include <dory/core/services/layoutService.h>
#include <cmath>

namespace dory::core::services
{
    using namespace resources;
    using namespace resources::scene;
    using namespace resources::scene::configuration;
    using namespace resources::scene::configuration;

    struct DefinedSize
    {
        std::optional<int> width {};
        std::optional<int> height {};
    };

    struct HorizontalLinePolicies
    {
        static constexpr std::size_t objects::layout::Position::* xProperty = &objects::layout::Position::x;
        static constexpr std::size_t objects::layout::Position::* yProperty = &objects::layout::Position::y;
        static constexpr std::size_t objects::layout::Size::* widthProperty = &objects::layout::Size::width;
        static constexpr std::size_t objects::layout::Size::* heightProperty = &objects::layout::Size::height;

        static constexpr std::optional<layout::Dimension> layout::Position::* xDefinitionProperty = &layout::Position::x;
        static constexpr std::optional<layout::Dimension> layout::Position::* yDefinitionProperty = &layout::Position::y;
        static constexpr std::optional<layout::Dimension> layout::Size::* widthDefinitionProperty = &layout::Size::width;
        static constexpr std::optional<layout::Dimension> layout::Size::* heightDefinitionProperty = &layout::Size::height;

        static constexpr std::optional<std::vector<layout::ContainerDefinition>> layout::ContainerDefinition::* childrenProperty = &layout::ContainerDefinition::horizontal;
    };

    struct VerticalLinePolicies
    {
        static constexpr std::size_t objects::layout::Position::* xProperty = &objects::layout::Position::y;
        static constexpr std::size_t objects::layout::Position::* yProperty = &objects::layout::Position::x;
        static constexpr std::size_t objects::layout::Size::* widthProperty = &objects::layout::Size::height;
        static constexpr std::size_t objects::layout::Size::* heightProperty = &objects::layout::Size::width;

        static constexpr std::optional<layout::Dimension> layout::Position::* xDefinitionProperty = &layout::Position::y;
        static constexpr std::optional<layout::Dimension> layout::Position::* yDefinitionProperty = &layout::Position::x;
        static constexpr std::optional<layout::Dimension> layout::Size::* widthDefinitionProperty = &layout::Size::height;
        static constexpr std::optional<layout::Dimension> layout::Size::* heightDefinitionProperty = &layout::Size::width;

        static constexpr std::optional<std::vector<layout::ContainerDefinition>> layout::ContainerDefinition::* childrenProperty = &layout::ContainerDefinition::vertical;
    };

    void calculateLayout(const layout::ContainerDefinition& definition, objects::layout::Container& container, std::size_t parentWidth, std::size_t parentHeight);

    template<typename T>
    bool isDefined(const std::optional<T>& optionalValue)
    {
        return optionalValue.has_value();
    }

    inline bool isDefined(const layout::Dimension& dimension)
    {
        return dimension.percents.has_value() || dimension.pixels.has_value();
    }

    std::size_t getDimensionValue(const layout::Dimension& dimension, const std::size_t wholeValue, const std::size_t defaultValue = 0)
    {
        std::size_t result = defaultValue;

        if(dimension.percents)
        {
            const float value = static_cast<float>(wholeValue) * (*dimension.percents * 0.01f);
            result = std::round(value);
        }
        else if(dimension.pixels)
        {
            result = *dimension.pixels;
        }

        return result;
    }

    inline std::size_t getContainerSizeDimension(const std::optional<layout::Dimension>& definition, const std::size_t parentSize, const std::size_t availableSize)
    {
        std::size_t result { availableSize };

        if(isDefined(definition))
        {
            result = getDimensionValue(*definition, parentSize);
        }

        return result;
    }

    std::size_t getCenteredPosition(const std::size_t size, const std::size_t parentSize)
    {
        std::size_t position { 0 };

        if(parentSize >= size)
        {
            position = std::floor(static_cast<float>(parentSize - size) / 2);
        }

        return position;
    }

    std::size_t getContainerPositionDimension(const std::optional<layout::Dimension>& positionDefinition,
        const std::size_t dimensionSize, const std::size_t parentSize)
    {
        std::size_t result {};

        if(isDefined(positionDefinition))
        {
            result = getDimensionValue(*positionDefinition, parentSize, 0);
        }
        else
        {
            result = getCenteredPosition(dimensionSize, parentSize);
        }

        return result;
    }

    template<typename TPolicies>
    void setContainerPosition(const layout::ContainerDefinition& containerDefinition,
        objects::layout::Container& container,
        const std::size_t parentWidth, const std::size_t parentHeight)
    {
        auto& x = container.position.*TPolicies::xProperty;
        auto& y = container.position.*TPolicies::yProperty;
        const auto& width = container.size.*TPolicies::widthProperty;
        const auto& height = container.size.*TPolicies::heightProperty;

        if(const auto position = containerDefinition.position; isDefined(position))
        {
            x = getContainerPositionDimension(*position.*TPolicies::xDefinitionProperty, width, parentWidth);
            y = getContainerPositionDimension(*position.*TPolicies::yDefinitionProperty, height, parentHeight);
        }
        else
        {
            x = getCenteredPosition(width, parentWidth);
            y = getCenteredPosition(height, parentHeight);
        }
    }

    template<typename TPolicies>
    void setContainerSize(const std::optional<layout::Size>& sizeDefinition, objects::layout::Size& size,
        const std::size_t parentWidth, const std::size_t parentHeight,
        const std::size_t availableWidth, const std::size_t availableHeight)
    {
        auto& width = size.*TPolicies::widthProperty;
        auto& height = size.*TPolicies::heightProperty;

        if(isDefined(sizeDefinition))
        {
            width = getContainerSizeDimension(*sizeDefinition.*TPolicies::widthDefinitionProperty, parentWidth, availableWidth);
            height = getContainerSizeDimension(*sizeDefinition.*TPolicies::heightDefinitionProperty, parentHeight, availableHeight);
        }
        else
        {
            width = availableWidth;
            height = availableHeight;
        }
    }

    template<typename TPolicies>
    void buildLine(const std::vector<layout::ContainerDefinition>& definitions, objects::layout::Container& container)
    {
        const auto& containerWidth = container.size.*TPolicies::widthProperty;
        const auto& containerHeight = container.size.*TPolicies::heightProperty;

        //if row's width has a non-zero value, we divide it between the row's columns and take all available height of the row
        std::optional<std::size_t> flexibleChildIndex {};
        std::size_t actualWidth {};
        for(std::size_t i= 0; i < definitions.size(); ++i)
        {
            const auto& childDefinition = definitions[i];
            auto& child = container.children.emplace_back();
            child.name = childDefinition.name;
            auto& width = child.size.*TPolicies::widthProperty;
            auto& height = child.size.*TPolicies::heightProperty;

            const auto& sizeDefinition = childDefinition.size;
            if(isDefined(sizeDefinition))
            {
                const auto& widthDefinition = *sizeDefinition.*TPolicies::widthDefinitionProperty;
                if(isDefined(widthDefinition))
                {
                    width = getDimensionValue(*widthDefinition, containerWidth);
                    height = getContainerSizeDimension(*sizeDefinition.*TPolicies::heightDefinitionProperty, containerHeight, containerHeight);
                    calculateLayout(childDefinition, child, containerWidth, containerHeight);

                    actualWidth += width;
                }
                else
                {
                    flexibleChildIndex = i;
                }
            }
            else
            {
                flexibleChildIndex = i;
            }
        }

        if(flexibleChildIndex)
        {
            //process the latest flexible-width column
            const auto & childDefinition = definitions[*flexibleChildIndex];
            objects::layout::Container& child = container.children[*flexibleChildIndex];
            auto& width = child.size.*TPolicies::widthProperty;
            auto& height = child.size.*TPolicies::heightProperty;

            const std::size_t columnWidth = containerWidth >= actualWidth ? containerWidth - actualWidth : 0;
            width = columnWidth;
            const auto& sizeDefinition = childDefinition.size;
            if(isDefined(sizeDefinition))
            {
                height = getContainerSizeDimension(*sizeDefinition.*TPolicies::heightDefinitionProperty, containerHeight, containerHeight);
            }
            else
            {
                height = containerHeight;
            }
            calculateLayout(childDefinition, child, containerWidth, containerHeight);

            if(columnWidth == 0)
            {
                //if column's width was zero before calling calculateLayout, it has to stay zero in order to not mess up the width of the row
                //the column possible was stretched by its contents
                width = 0;
            }
        }

        //set x-positions
        std::size_t currentWidth {};
        for(objects::layout::Container& child : container.children)
        {
            child.position.*TPolicies::xProperty = currentWidth;
            currentWidth += child.size.*TPolicies::widthProperty;
        }
    }

    template<typename TPolicies>
    void buildTiles(const std::vector<layout::ContainerDefinition>& definitions, objects::layout::Container& container, const std::size_t lineMaxWidth)
    {
        //if row's width value is zero, we stretch the row with columns and if parent's width
        //is non-zero also, we wrap the lines of columns in order to fill the available space
        //by width and by height, otherwise the columns will be placed in a single line

        std::size_t currentX {};
        std::size_t currentY {};
        std::size_t currentHeight {};
        std::size_t currentWidth {};
        for(const layout::ContainerDefinition& childDefinition : definitions)
        {
            auto& child = container.children.emplace_back();
            child.name = childDefinition.name;

            auto& x = child.position.*TPolicies::xProperty;
            auto& y = child.position.*TPolicies::yProperty;
            const auto& width = child.size.*TPolicies::widthProperty;
            const auto& height = child.size.*TPolicies::heightProperty;

            setContainerSize<TPolicies>(childDefinition.size, child.size, 0, container.size.*TPolicies::heightProperty, 0, 0);

            calculateLayout(childDefinition, child, container.size.width, container.size.height);

            if(lineMaxWidth > 0 && currentX + width > lineMaxWidth)
            {
                currentX = 0;
                currentY = currentHeight;
            }

            x = currentX;
            y = currentY;

            currentX += width;

            if(const size_t extent = height + y; extent > currentHeight)
            {
                currentHeight = extent;
            }
            if(const size_t extent = width + x; extent > currentWidth)
            {
                currentWidth = extent;
            }
        }

        container.size.*TPolicies::widthProperty = currentWidth;
    }

    template<typename TPolicies>
    void calculateInnerHeight(objects::layout::Container& container)
    {
        //calculate actual height of the row in case of has to be defined be its content height
        auto& containerHeight = container.size.*TPolicies::heightProperty;
        if(containerHeight == 0)
        {
            for(const objects::layout::Container& child : container.children)
            {
                const size_t height = child.position.*TPolicies::yProperty + child.size.*TPolicies::heightProperty;
                if(height > containerHeight)
                {
                    containerHeight = height;
                }
            }
        }
    }

    void processDetachedContainer(const layout::ContainerDefinition& containerDefinition,
        objects::layout::Container& container,
        const std::size_t parentWidth, const std::size_t parentHeight)
    {
        container.name = containerDefinition.name;
        setContainerSize<HorizontalLinePolicies>(containerDefinition.size, container.size, parentWidth, parentHeight, parentWidth, parentHeight);
        calculateLayout(containerDefinition, container, parentWidth, parentHeight);
        setContainerPosition<HorizontalLinePolicies>(containerDefinition, container, parentWidth, parentHeight);
    }

    template<typename TPolicies>
    void calculateGridLayout(const std::vector<layout::ContainerDefinition>& definitions, objects::layout::Container& container, std::size_t parentWidth)
    {
        const auto& containerWidth = container.size.*TPolicies::widthProperty;
        const auto& containerHeight = container.size.*TPolicies::heightProperty;

        if(containerWidth > 0)
        {
            buildLine<TPolicies>(definitions, container);
        }
        else
        {
            buildTiles<TPolicies>(definitions, container, parentWidth);
        }

        if(containerHeight == 0)
        {
            calculateInnerHeight<TPolicies>(container);
        }
    }

    void calculateLayout(const layout::ContainerDefinition& definition, objects::layout::Container& container, const std::size_t parentWidth, const std::size_t parentHeight)
    {
        //it might be confusing, but despite container's width and height are const in this function,
        //they might be updated in the sub-functions called from this function and change their value eventually
        const auto& containerWidth = container.size.width;
        const auto& containerHeight = container.size.height;

        if(definition.horizontal)
        {
            calculateGridLayout<HorizontalLinePolicies>(*definition.horizontal, container, parentWidth);
        }
        else if(definition.vertical)
        {
            calculateGridLayout<VerticalLinePolicies>(*definition.vertical, container, parentHeight);
        }

        //process detached containers
        if(definition.positioned)
        {
            for(const layout::ContainerDefinition& childDefinition : *definition.positioned)
            {
                objects::layout::Container& child = container.children.emplace_back();
                processDetachedContainer(childDefinition, child, containerWidth, containerHeight);
            }
        }
    }

    objects::layout::Container calculateWithoutRecursion(const layout::ContainerDefinition& layoutDefinition, const objects::layout::Size& availableSpace);

    objects::layout::Container LayoutService::calculate(const layout::ContainerDefinition& layoutDefinition, const objects::layout::Size& availableSpace)
    {
        objects::layout::Container container;
        processDetachedContainer(layoutDefinition, container, availableSpace.width, availableSpace.height);

        return container;
    }

    /*---------------------------------------------------non-recursive implementation attempt---------------------------------------------------*/

#include <stack>

    template<typename TPolicies>
    void buildContainerLines(const layout::ContainerDefinition& containerDefinition, objects::layout::Container& rootContainer, const objects::layout::Size& availableSpace)
    {
        std::stack<std::tuple<const layout::ContainerDefinition*, objects::layout::Container*, const objects::layout::Size*, objects::layout::Size*>> stack;
        stack.emplace(&containerDefinition, &rootContainer, &availableSpace, &rootContainer.actualSize);

        while(!stack.empty())
        {
            auto& [currentDefinition, currentContainer, parentSize, parentActualSize] = stack.top();
            stack.pop();

            currentContainer->name = currentDefinition->name;

            const auto& parentWidth = *parentSize.*TPolicies::widthProperty;
            const auto& parentHeight = *parentSize.*TPolicies::heightProperty;
            auto& parentActualWidth = *parentActualSize.*TPolicies::widthProperty;

            setContainerSize<TPolicies>(currentDefinition->size, currentContainer->size, parentWidth, parentHeight, 0/*do not stretch container*/, 0);

            //set flexible container size
            auto& sizeDefinition = currentDefinition->size;
            if(!isDefined(sizeDefinition) || !isDefined(*sizeDefinition.*TPolicies::widthDefinitionProperty))
            {
                //use available parent's width
                const std::size_t columnWidth = parentWidth >= parentActualWidth ? parentWidth - parentActualWidth : 0;
                currentContainer->size.*TPolicies::widthProperty = columnWidth;
            }

            parentActualWidth += currentContainer->size.*TPolicies::widthProperty;

            //sort children on fixed size and flexible siblings
            if(auto& children = *currentDefinition.*TPolicies::childrenProperty)
            {
                using SiblingEntry = std::tuple<const layout::ContainerDefinition*, objects::layout::Container*>;
                std::vector<SiblingEntry> siblings(children.value().size()); //reserve memory for all children
                std::optional<SiblingEntry> flexibleSibling {};
                for(const auto& childDefinition : *children)
                {
                    auto& childContainer = currentContainer->children.emplace_back();

                    //skip flexible containers, because their children do not know the size of the parent yet to calculate percents
                    auto& childSizeDefinition = childDefinition.size;
                    if(isDefined(childSizeDefinition) && isDefined(*childSizeDefinition.*TPolicies::widthDefinitionProperty))
                    {
                        siblings.emplace_back(&childDefinition, &childContainer);
                    }
                    else
                    {
                        flexibleSibling = SiblingEntry{ &childDefinition, &childContainer };
                    }
                }

                //push flexible sibling on stack first, then fixed-width siblings
                if(flexibleSibling)
                {
                    auto [childDefinition, childContainer] = *flexibleSibling;
                    stack.emplace(childDefinition, childContainer, &currentContainer->size, &currentContainer->actualSize);
                }

                for(auto [childDefinition, childContainer] : siblings)
                {
                    stack.emplace(childDefinition, childContainer, &currentContainer->size, &currentContainer->actualSize);
                }
            }
        }
    }

    template<typename TPolicies>
    void buildContainerTiles(const layout::ContainerDefinition& containerDefinition, objects::layout::Container& rootContainer, const objects::layout::Size& availableSpace)
    {

    }

    objects::layout::Container calculateWithoutRecursion(const layout::ContainerDefinition& layoutDefinition, const objects::layout::Size& availableSpace)
    {
        objects::layout::Container container;

        buildContainerLines<HorizontalLinePolicies>(layoutDefinition, container, availableSpace);
        buildContainerLines<VerticalLinePolicies>(layoutDefinition, container, availableSpace);

        // * build tiles
        // * calculate flexible containers
        // *

        return container;

        /*const std::size_t parentWidth = availableSpace.width;
        const std::size_t parentHeight = availableSpace.height;

        container.name = layoutDefinition.name;
        setContainerSize<HorizontalLinePolicies>(layoutDefinition.size, container.size, parentWidth, parentHeight, parentWidth, parentHeight);

        while(!stack.empty())
        {
            const layout::ContainerDefinition* currentDefinition = stack.top();
            stack.pop();

            if(currentDefinition->horizontal)
            {
                const auto& containerWidth = container.size.*HorizontalLinePolicies::widthProperty;
                const auto& containerHeight = container.size.*HorizontalLinePolicies::heightProperty;

                const auto& definitions = *currentDefinition->horizontal;

                if(containerWidth > 0)
                {
                    //if row's width has a non-zero value, we divide it between the row's columns and take all available height of the row
                    std::optional<std::size_t> flexibleChildIndex {};
                    std::size_t actualWidth {};
                    for(std::size_t i= 0; i < definitions.size(); ++i)
                    {
                        const auto& childDefinition = definitions[i];
                        auto& child = container.children.emplace_back();
                        child.name = childDefinition.name;
                        auto& width = child.size.*HorizontalLinePolicies::widthProperty;
                        auto& height = child.size.*HorizontalLinePolicies::heightProperty;

                        const auto& sizeDefinition = childDefinition.size;
                        if(isDefined(sizeDefinition))
                        {
                            const auto& widthDefinition = *sizeDefinition.*HorizontalLinePolicies::widthDefinitionProperty;
                            if(isDefined(widthDefinition))
                            {
                                width = getDimensionValue(*widthDefinition, containerWidth);
                                height = getContainerSizeDimension(*sizeDefinition.*HorizontalLinePolicies::heightDefinitionProperty, containerHeight, containerHeight);

                                calculateLayout(childDefinition, child, containerWidth, containerHeight);

                                actualWidth += width;
                            }
                            else
                            {
                                flexibleChildIndex = i;
                            }
                        }
                        else
                        {
                            flexibleChildIndex = i;
                        }
                    }

                    if(flexibleChildIndex)
                    {
                        //process the latest flexible-width column
                        const auto & childDefinition = definitions[*flexibleChildIndex];
                        objects::layout::Container& child = container.children[*flexibleChildIndex];
                        auto& width = child.size.*HorizontalLinePolicies::widthProperty;
                        auto& height = child.size.*HorizontalLinePolicies::heightProperty;

                        const std::size_t columnWidth = containerWidth >= actualWidth ? containerWidth - actualWidth : 0;
                        width = columnWidth;
                        const auto& sizeDefinition = childDefinition.size;
                        if(isDefined(sizeDefinition))
                        {
                            height = getContainerSizeDimension(*sizeDefinition.*HorizontalLinePolicies::heightDefinitionProperty, containerHeight, containerHeight);
                        }
                        else
                        {
                            height = containerHeight;
                        }
                        calculateLayout(childDefinition, child, containerWidth, containerHeight);

                        if(columnWidth == 0)
                        {
                            //if column's width was zero before calling calculateLayout, it has to stay zero in order to not mess up the width of the row
                            //the column possible was stretched by its contents
                            width = 0;
                        }
                    }

                    //set x-positions
                    std::size_t currentWidth {};
                    for(objects::layout::Container& child : container.children)
                    {
                        child.position.*HorizontalLinePolicies::xProperty = currentWidth;
                        currentWidth += child.size.*HorizontalLinePolicies::widthProperty;
                    }
                }
                else
                {
                    //buildTiles<TPolicies>(definitions, container, parentWidth);
                }

                if(containerHeight == 0)
                {
                    //calculateInnerHeight<TPolicies>(container);
                }
            }
            else if(currentDefinition->vertical)
            {
                calculateGridLayout<VerticalLinePolicies>(*currentDefinition->vertical, container, parentHeight);
            }

            //process detached containers
            if(currentDefinition->positioned)
            {
                for(const layout::ContainerDefinition& childDefinition : *currentDefinition->positioned)
                {
                    objects::layout::Container& child = container.children.emplace_back();
                    processDetachedContainer(childDefinition, child, containerWidth, containerHeight);
                }
            }
        }

        setContainerPosition<HorizontalLinePolicies>(layoutDefinition, container, parentWidth, parentHeight);

        return container;*/
    }
}
