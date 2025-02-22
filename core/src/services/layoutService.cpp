#include <dory/core/services/layoutService.h>
#include <cmath>

namespace dory::core::services
{
    using namespace resources;
    using namespace resources::scene;
    using namespace resources::scene::configuration;
    using namespace resources::scene::configuration;

    void calculateColumnLayout(const layout::FloatingContainer& definition, objects::layout::Container& column, std::size_t parentHeight);
    void calculateRowLayout(const layout::FloatingContainer& definition, objects::layout::Container& row, std::size_t parentWidth);

    struct DefinedSize
    {
        std::optional<int> width {};
        std::optional<int> height {};
    };

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
            result = std::floor(static_cast<float>(wholeValue) * (*dimension.percents * 0.01f));
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

    void setContainerPositionDimension(const std::optional<layout::Dimension>& positionDefinition,
        std::size_t& position, const std::size_t dimensionSize, const std::size_t parentSize)
    {
        if(isDefined(positionDefinition))
        {
            position = getDimensionValue(*positionDefinition, parentSize, 0);
        }
        else
        {
            position = getCenteredPosition(dimensionSize, parentSize);
        }
    }

    void setContainerPosition(const layout::PositionedContainer& containerDefinition,
        objects::layout::Container& container,
        const std::size_t parentWidth, const std::size_t parentHeight)
    {
        if(const auto position = containerDefinition.position; isDefined(position))
        {
            setContainerPositionDimension(position->x, container.position.x, container.size.width, parentWidth);
            setContainerPositionDimension(position->y, container.position.y, container.size.height, parentHeight);
        }
        else
        {
            container.position.x = getCenteredPosition(container.size.width, parentWidth);
            container.position.y = getCenteredPosition(container.size.height, parentHeight);
        }
    }

    void setContainerSize(const std::optional<layout::Size>& sizeDefinition, objects::layout::Size& size,
        const std::size_t parentWidth, const std::size_t parentHeight,
        const std::size_t availableWidth, const std::size_t availableHeight)
    {
        if(isDefined(sizeDefinition))
        {
            getContainerSizeDimension(sizeDefinition->width, parentWidth, availableWidth);
            getContainerSizeDimension(sizeDefinition->height, parentHeight, availableHeight);
        }
        else
        {
            size.width = availableWidth;
            size.height = availableHeight;
        }
    }

    void processDetachedContainer(const layout::PositionedContainer& containerDefinition,
        objects::layout::Container& container,
        const std::size_t parentWidth, const std::size_t parentHeight)
    {
        container.name = containerDefinition.name;
        setContainerSize(containerDefinition.size, container.size, parentWidth, parentHeight, parentWidth, parentHeight);
        calculateRowLayout(containerDefinition, container, parentWidth);
        setContainerPosition(containerDefinition, container, parentWidth, parentHeight);
    }

    struct HorizontalLinePolicies
    {
        static constexpr std::size_t objects::layout::Position::* xProperty = &objects::layout::Position::x;
        static constexpr std::size_t objects::layout::Position::* yProperty = &objects::layout::Position::y;
        static constexpr std::size_t objects::layout::Size::* widthProperty = &objects::layout::Size::width;
        static constexpr std::size_t objects::layout::Size::* heightProperty = &objects::layout::Size::height;
    };

    struct VerticalLinePolicies
    {
        static constexpr std::size_t objects::layout::Position::* xProperty = &objects::layout::Position::y;
        static constexpr std::size_t objects::layout::Position::* yProperty = &objects::layout::Position::x;
        static constexpr std::size_t objects::layout::Size::* widthProperty = &objects::layout::Size::height;
        static constexpr std::size_t objects::layout::Size::* heightProperty = &objects::layout::Size::width;
    };

    template<typename TPolicies>
    void buildTiles(const layout::FloatingContainer& definition, objects::layout::Container& container, const std::size_t lineMaxWidth)
    {
        std::size_t currentX {};
        std::size_t currentY {};
        std::size_t currentHeight {};
        std::size_t currentWidth {};
        for(const layout::FloatingContainer& columnDefinition : definition.children)
        {
            auto& column = container.children.emplace_back();
            column.name = columnDefinition.name;

            auto& x = column.position.*TPolicies::xProperty;
            auto& y = column.position.*TPolicies::yProperty;
            const auto& width = column.size.*TPolicies::widthProperty;
            const auto& height = column.size.*TPolicies::heightProperty;

            //TODO: generalize width and height properties
            setContainerSize(columnDefinition.size, column.size, 0, container.size.height, 0, 0);

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
        container.size.*TPolicies::heightProperty = currentHeight;
    }

    /*
     * if width or height are 0 in the row.size, it means those dimensions are defined by the content size(or there is no space for the row in the parent)
     */
    void calculateRowLayout(const layout::FloatingContainer& definition, objects::layout::Container& row, std::size_t parentWidth)
    {
        if(row.size.width > 0)
        {
            //if row's width has a non-zero value, we divide it between the row's columns and take all available height of the row
            std::optional<std::size_t> flexibleChildIndex {};
            std::size_t actualWidth {};
            for(std::size_t i= 0; i < definition.children.size(); ++i)
            {
                const auto& columnDefinition = definition.children[i];
                auto& column = row.children.emplace_back();
                column.name = columnDefinition.name;

                if(isDefined(columnDefinition.size) && isDefined(columnDefinition.size->width))
                {
                    column.size.width = getDimensionValue(*columnDefinition.size->width, row.size.width);
                    if(isDefined(columnDefinition.size))
                    {
                        column.size.height = getContainerSizeDimension(columnDefinition.size->height, row.size.height, row.size.height);
                    }
                    calculateColumnLayout(columnDefinition, column, row.size.height);

                    actualWidth += column.size.width;
                }
                else
                {
                    flexibleChildIndex = i;
                }
            }

            if(flexibleChildIndex)
            {
                //process the latest flexible-width column
                const auto & columnDefinition = definition.children[*flexibleChildIndex];
                objects::layout::Container column = row.children[*flexibleChildIndex];

                const std::size_t columnWidth = row.size.width >= actualWidth ? row.size.width - actualWidth : 0;
                column.size.width = columnWidth;
                if(isDefined(columnDefinition.size))
                {
                    column.size.height = getContainerSizeDimension(columnDefinition.size->height, row.size.height, row.size.height);
                }
                calculateColumnLayout(columnDefinition, column, row.size.height);

                if(columnWidth == 0)
                {
                    //if column's width was zero before calling calculateLayout, it has to stay zero in order to not mess up the width of the row
                    //the column possible was stretched by its contents
                    column.size.width = 0;
                }
            }

            //set x-positions
            std::size_t currentWidth {};
            for(objects::layout::Container& column : row.children)
            {
                column.position.x = currentWidth;
                currentWidth += column.size.width;
            }
        }
        else
        {
            //if row's width value is zero, we stretch the row with columns and if parent's width
            //is non-zero also, we wrap the lines of columns in order to fill the available space
            //by width and by height, otherwise the columns will be placed in a single line

            buildTiles<HorizontalLinePolicies>(definition, row, parentWidth);
        }

        //calculate actual height of the row in case of has to be defined be its content height
        if(row.size.height == 0)
        {
            for(const objects::layout::Container& column : row.children)
            {
                const size_t height = column.position.x + column.size.height;
                if(height > row.size.height)
                {
                    row.size.height = height;
                }
            }
        }

        //process detached containers
        for(const layout::PositionedContainer& containerDefinition : definition.positionedContainers)
        {
            objects::layout::Container& container = row.children.emplace_back();
            processDetachedContainer(containerDefinition, container, row.size.width, row.size.height);
        }
    }

    void calculateLayout(const layout::FloatingContainer& definition, objects::layout::Container& column, std::size_t parentHeight)
    {

    }

    objects::layout::Container LayoutService::calculate(const layout::PositionedContainer& layoutDefinition, const objects::layout::Size& availableSpace)
    {
        objects::layout::Container container;
        processDetachedContainer(layoutDefinition, container, availableSpace.width, availableSpace.height);

        return container;
    }
}
