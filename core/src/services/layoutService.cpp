#include <dory/core/services/layoutService.h>
#include <cmath>

namespace dory::core::services
{
    using namespace resources;
    using namespace resources::scene;
    using namespace resources::scene::configuration;
    using namespace resources::scene::configuration;

    void calculateLayout(const layout::Column& definition, objects::layout::Column& column, std::size_t parentHeight);
    void calculateLayout(const layout::PositionedContainer& definition, objects::layout::DetachedContainer& container,
        std::size_t parentWidth, std::size_t parentHeight);

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

    inline std::size_t getColumnHeight(const layout::Column& definition, const std::size_t rowHeight)
    {
        std::size_t result { rowHeight };

        if(isDefined(definition.size) && isDefined(definition.size->height))
        {
            result = getDimensionValue(*definition.size->height, rowHeight, 0);
        }

        return result;
    }

    void processContainer(const layout::PositionedContainer& containerDefinition,
        objects::layout::DetachedContainer& container,
        const std::size_t parentWidth, const std::size_t parentHeight)
    {
        container.name = containerDefinition.name;

        if(isDefined(containerDefinition.size))
        {
            if(isDefined(containerDefinition.size->width))
            {
                container.size.width = getDimensionValue(*containerDefinition.size->width, parentWidth);
            }

            if(isDefined(containerDefinition.size->height))
            {
                container.size.height = getDimensionValue(*containerDefinition.size->height, parentHeight);
            }
        }

        calculateLayout(containerDefinition, container, parentWidth, parentHeight);

        //TODO: calculate position: fixed, auto center, origin
    }

    /*
     * if width or height are 0 in the row.size, it means those dimensions are defined by the content size(or there is no space for the row in the parent)
     */
    void calculateLayout(const layout::Row& definition, objects::layout::Row& row, std::size_t parentWidth)
    {
        if(row.size.width > 0)
        {
            //if row's width has a non-zero value, we divide it between the row's columns and take all available height of the row
            std::optional<std::size_t> flexibleChildIndex {};
            std::size_t actualWidth {};
            for(std::size_t i= 0; i < definition.columns.size(); ++i)
            {
                const auto& columnDefinition = definition.columns[i];
                auto column = row.columns.emplace_back();
                column.name = columnDefinition.name;

                if(isDefined(columnDefinition.size) && isDefined(columnDefinition.size->width))
                {
                    column.size.width = getDimensionValue(*columnDefinition.size->width, row.size.width);
                    column.size.height = getColumnHeight(columnDefinition, row.size.height);
                    calculateLayout(columnDefinition, column, row.size.height);

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
                const auto & columnDefinition = definition.columns[*flexibleChildIndex];
                objects::layout::Column column = row.columns[*flexibleChildIndex];

                const std::size_t columnWidth = row.size.width >= actualWidth ? row.size.width - actualWidth : 0;
                column.size.width = columnWidth;
                column.size.height = getColumnHeight(columnDefinition, row.size.height);
                calculateLayout(columnDefinition, column, row.size.height);

                if(columnWidth == 0)
                {
                    //if column's width was zero before calling calculateLayout, it has to stay zero in order to not mess up the width of the row
                    //the column possible was stretched by its contents
                    column.size.width = 0;
                }
            }

            //set x-positions
            std::size_t currentWidth {};
            for(objects::layout::Column& column : row.columns)
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

            //TODO: implement line-wrap mode when the row cannot exceed parent's width, but its own width is defined by the width of the inner columns

            //TODO: set column positions
        }

        //calculate actual height of the row in case of has to be defined be its content height
        if(row.size.height == 0)
        {
            for(const objects::layout::Column& column : row.columns)
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
            objects::layout::DetachedContainer container = row.detachedContainers.emplace_back();
            processContainer(containerDefinition, container, row.size.width, row.size.height);
        }
    }

    void calculateLayout(const layout::Column& definition, objects::layout::Column& column, std::size_t parentHeight)
    {

    }

    void calculateLayout(const layout::PositionedContainer& definition, objects::layout::DetachedContainer& container,
        const std::size_t parentWidth, const std::size_t parentHeight)
    {
        //TODO: calculate inner layout(rows)
    }

    objects::layout::DetachedContainer LayoutService::calculate(const layout::PositionedContainer& layoutDefinition, const objects::layout::Size& availableSpace)
    {
        objects::layout::DetachedContainer container;
        processContainer(layoutDefinition, container, availableSpace.width, availableSpace.height);

        return container;
    }
}
