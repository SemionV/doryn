#include <dory/core/services/layoutService.h>
#include <cmath>

namespace dory::core::services
{
    using namespace resources;
    using namespace resources::scene;
    using namespace resources::scene::configuration;
    using namespace resources::scene::configuration;

    objects::layout::Column calculateColumn(const layout::Column& definition, const objects::layout::Size& availableSpace);

    objects::layout::Row calculateRow(const layout::Row& definition, const objects::layout::Size& availableSpace)
    {
        std::optional<int> rowWidth {};
        std::optional<int> rowHeight {};

        if(definition.size)
        {
            if(definition.size->width)
            {
                if(definition.size->width->percents)
                {
                    rowWidth = std::floor(static_cast<float>(availableSpace.width) * (*definition.size->width->percents * 0.01f));
                }
                else if(definition.size->width->pixels)
                {
                    rowWidth = *definition.size->width->pixels;
                }
            }

            if(definition.size->height)
            {
                if(definition.size->height->percents)
                {
                    rowHeight = std::floor(static_cast<float>(availableSpace.height) * (*definition.size->height->percents * 0.01f));
                }
                else if(definition.size->height->pixels)
                {
                    rowHeight = *definition.size->height->pixels;
                }
            }
        }

        objects::layout::Size availableSpaceRow { };
        availableSpaceRow.width = rowWidth ? rowWidth.value() : availableSpace.width;
        availableSpaceRow.height = rowHeight ? rowHeight.value() : availableSpace.height;
        for(const auto& columnDefinition : definition.columns)
        {
            objects::layout::Column column = calculateColumn(columnDefinition, availableSpace);
            //TODO: subdue the column's dimensions from the leftover free space
            //TODO: implement line-wrapping, move columns to the next line
        }

        if(!rowWidth)
        {
            rowWidth = availableSpace.width - availableSpaceRow.width;
        }
        if(!rowHeight)
        {
            rowHeight = availableSpace.height - availableSpaceRow.height;
        }

        //both dimensions have to be calculated by this time
        assert(rowWidth);
        assert(rowHeight);

        objects::layout::Row row;

        row.size.width = *rowWidth;
        row.size.height = *rowHeight;

        return row;
    }

    objects::layout::Column calculateColumn(const layout::Column& definition, const objects::layout::Size& availableSpace)
    {
        std::optional<int> columnWidth {};
        std::optional<int> columnHeight {};

        objects::layout::Column column;

        column.size.width = *columnWidth;
        column.size.height = *columnHeight;

        return column;
    }

    objects::layout::Row LayoutService::calculate(const layout::Row& layoutDefinition, const resources::objects::layout::Size& availableSpace)
    {
        return calculateRow(layoutDefinition, availableSpace);
    }
}
