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
    };

    void calculateLayout(const layout::FloatingContainer& definition, objects::layout::Container& container, std::size_t parentWidth);

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
    void setContainerPosition(const layout::PositionedContainer& containerDefinition,
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

    void processDetachedContainer(const layout::PositionedContainer& containerDefinition,
        objects::layout::Container& container,
        const std::size_t parentWidth, const std::size_t parentHeight)
    {
        container.name = containerDefinition.name;
        setContainerSize<HorizontalLinePolicies>(containerDefinition.size, container.size, parentWidth, parentHeight, parentWidth, parentHeight);
        calculateLayout(containerDefinition, container, parentWidth);
        setContainerPosition<HorizontalLinePolicies>(containerDefinition, container, parentWidth, parentHeight);
    }

    template<typename TPolicies>
    void buildLine(const std::vector<layout::FloatingContainer>& definitions, objects::layout::Container& container)
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
                    if(isDefined(sizeDefinition))
                    {
                        height = getContainerSizeDimension(*sizeDefinition.*TPolicies::heightDefinitionProperty, containerHeight, containerHeight);
                    }
                    else
                    {
                        height = containerHeight;
                    }
                    calculateLayout(childDefinition, child, containerWidth);

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
            calculateLayout(childDefinition, child, containerWidth);

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
    void buildTiles(const std::vector<layout::FloatingContainer>& definitions, objects::layout::Container& container, const std::size_t lineMaxWidth)
    {
        //if row's width value is zero, we stretch the row with columns and if parent's width
        //is non-zero also, we wrap the lines of columns in order to fill the available space
        //by width and by height, otherwise the columns will be placed in a single line

        std::size_t currentX {};
        std::size_t currentY {};
        std::size_t currentHeight {};
        std::size_t currentWidth {};
        for(const layout::FloatingContainer& childDefinition : definitions)
        {
            auto& child = container.children.emplace_back();
            child.name = childDefinition.name;

            auto& x = child.position.*TPolicies::xProperty;
            auto& y = child.position.*TPolicies::yProperty;
            const auto& width = child.size.*TPolicies::widthProperty;
            const auto& height = child.size.*TPolicies::heightProperty;

            setContainerSize<TPolicies>(childDefinition.size, child.size, 0, container.size.*TPolicies::heightProperty, 0, 0);

            calculateLayout(childDefinition, child, container.size.*TPolicies::widthProperty);

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
                const size_t height = child.position.*TPolicies::xProperty + child.size.*TPolicies::heightProperty;
                if(height > containerHeight)
                {
                    containerHeight = height;
                }
            }
        }
    }

    template<typename TPolicies>
    void calculateGridLayout(const std::vector<layout::FloatingContainer>& definitions, objects::layout::Container& container, std::size_t parentWidth)
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

    void calculateLayout(const layout::FloatingContainer& definition, objects::layout::Container& container, std::size_t parentWidth)
    {
        //it might be confusing, but despite container's width and height are const in this function,
        //they might be updated in the sub-functions called from this function and change their value eventually
        const auto& containerWidth = container.size.width;
        const auto& containerHeight = container.size.height;

        if(!definition.horizontal.empty())
        {
            calculateGridLayout<HorizontalLinePolicies>(definition.horizontal, container, containerWidth);
        }
        else if(!definition.vertical.empty())
        {
            calculateGridLayout<VerticalLinePolicies>(definition.vertical, container, containerHeight);
        }

        //process detached containers
        for(const layout::PositionedContainer& childDefinition : definition.positioned)
        {
            objects::layout::Container& child = container.children.emplace_back();
            processDetachedContainer(childDefinition, child, containerWidth, containerHeight);
        }
    }

    objects::layout::Container LayoutService::calculate(const layout::PositionedContainer& layoutDefinition, const objects::layout::Size& availableSpace)
    {
        objects::layout::Container container;
        processDetachedContainer(layoutDefinition, container, availableSpace.width, availableSpace.height);

        return container;
    }
}
