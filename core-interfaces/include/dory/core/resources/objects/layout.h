#pragma once

#include "../name.h"
#include <array>
#include <optional>
#include <vector>

namespace dory::core::resources::objects::layout
{
    enum class Align
    {
        center
    };

    enum class Upstream
    {
        parent,
        fill,
        children
    };

    struct Dimension
    {
        std::optional<int> pixels {};
        std::optional<Name> variable {};
        std::optional<float> percents {};
    };

    struct DimensionPoint: public Dimension
    {
        std::optional<Align> align {};
    };

    struct DimensionSegment: public Dimension
    {
        std::optional<Upstream> upstream {};
    };

    struct ContainerDefinition
    {
        DimensionSegment width;
        DimensionSegment height;
        DimensionPoint x;
        DimensionPoint y;
        std::string name {};

        std::vector<ContainerDefinition> columns {};
        std::vector<ContainerDefinition> rows {};
        std::vector<ContainerDefinition> tileRow {};
        std::vector<ContainerDefinition> tileColumn {};
        std::vector<ContainerDefinition> floating {};
        std::vector<ContainerDefinition> slides {};

        DimensionPoint& getPosition(const std::size_t axis)
        {
            if(axis == 0)
            {
                return x;
            }

            return y;
        }

        DimensionSegment& getSize(const std::size_t axis)
        {
            if(axis == 0)
            {
                return width;
            }

            return height;
        }

        ContainerDefinition() = default;

        explicit ContainerDefinition(const std::string& name, const DimensionPoint& x, const DimensionPoint& y,
            const DimensionSegment& width, const DimensionSegment& height):
            width(width), height(height), x(x), y(y), name(name)
        {}
    };

    using Vector2i = std::array<int, 2>;
    using Axes2D = std::array<std::size_t, 2>;

    struct Axes
    {
        static constexpr std::size_t x = 0;
        static constexpr std::size_t y = 1;

        static constexpr auto xy = std::array{x, y};
        static constexpr auto yx = std::array{y, x};
    };

    enum class AlignOrder
    {
        center
    };

    struct DimensionValue
    {
        std::optional<Name> variable {};
        std::optional<float> percents {};
        std::optional<int> pixels {};
    };

    struct PositionValue: public DimensionValue
    {
        std::optional<AlignOrder> order { };
    };

    struct SizeValue: public DimensionValue
    {
        std::optional<Upstream> upstream {};
    };

    struct Alignment
    {
        Axes2D axes{};
        std::optional<std::array<PositionValue, 2>> fixedPosition;
        bool lineWrap {};
        bool floating {};
    };

    struct Stretching
    {
        std::array<SizeValue, 2> axes {};
    };

    struct NodeItemSetup
    {
        Name name {};
        std::size_t parent {};
        std::vector<std::size_t> children{};
        Alignment alignment {};
        Stretching stretching {};
    };

    struct LineCursor
    {
        Vector2i ul {}; //upper-left corner
        Vector2i br {}; //bottom-right corner
    };

    struct NodeItemState
    {
        LineCursor cursor {};
        Vector2i pos {};
        Vector2i dim {};
    };

    struct NodeSetupList
    {
        std::vector<NodeItemSetup> nodes {};
    };

    struct NodeStateList
    {
        std::vector<NodeItemState> nodes {};
    };

    //a dummy class to represent variables storage
    struct Variables
    {};

    namespace util
    {
        using us = Upstream;
        using al = Align;

        inline ContainerDefinition def(const Name& name)
        {
            ContainerDefinition result;
            result.name = name;
            return result;
        }

        inline ContainerDefinition def()
        {
            return ContainerDefinition {};
        }

        template <typename Func>
        ContainerDefinition operator|(ContainerDefinition container, Func func)
        {
            return func(container);
        }

        inline void setValue(Dimension& dim, const int value)
        {
            dim.pixels = value;
        }

        inline void setValue(Dimension& dim, const float value)
        {
            dim.percents = value;
        }

        inline void setValue(Dimension& dim, const Name& value)
        {
            dim.variable = value;
        }

        template<typename T>
        void setPoint(DimensionPoint& dim, const T& value)
        {
            setValue(dim, value);
        }

        inline void setPoint(DimensionPoint& dim, const Align value)
        {
            dim.align = value;
        }

        template<typename T>
        void setSize(DimensionSegment& dim, const T& value)
        {
            setValue(dim, value);
        }

        inline void setSize(DimensionSegment& dim, const Upstream value)
        {
            dim.upstream = value;
        }

        template<typename T>
        auto x(const T& value) {
            return [value](ContainerDefinition& container) -> ContainerDefinition& {
                setPoint(container.x, value);
                return container;
            };
        }

        template<typename T>
        auto y(const T& value) {
            return [value](ContainerDefinition& container) -> ContainerDefinition& {
                setPoint(container.y, value);
                return container;
            };
        }

        template<typename T>
        auto w(const T& value) {
            return [value](ContainerDefinition& container) -> ContainerDefinition& {
                setSize(container.width, value);
                return container;
            };
        }

        template<typename T>
        auto h(const T& value) {
            return [value](ContainerDefinition& container) -> ContainerDefinition& {
                setSize(container.height, value);
                return container;
            };
        }

        inline auto columns(const std::vector<ContainerDefinition>& items) {
            return [items](ContainerDefinition& container) -> ContainerDefinition& {
                container.columns = items;
                return container;
            };
        }

        inline auto rows(const std::vector<ContainerDefinition>& items) {
            return [items](ContainerDefinition& container) -> ContainerDefinition& {
                container.rows = items;
                return container;
            };
        }

        inline auto rowTiles(const std::vector<ContainerDefinition>& items) {
            return [items](ContainerDefinition& container) -> ContainerDefinition& {
                container.tileRow = items;
                return container;
            };
        }

        inline auto columnTiles(const std::vector<ContainerDefinition>& items) {
            return [items](ContainerDefinition& container) -> ContainerDefinition& {
                container.tileColumn = items;
                return container;
            };
        }

        inline auto slides(const std::vector<ContainerDefinition>& items) {
            return [items](ContainerDefinition& container) -> ContainerDefinition& {
                container.slides = items;
                return container;
            };
        }

        inline auto floating(const std::vector<ContainerDefinition>& items) {
            return [items](ContainerDefinition& container) -> ContainerDefinition& {
                container.floating = items;
                return container;
            };
        }
    }
}