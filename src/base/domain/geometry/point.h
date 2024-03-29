#pragma once

namespace dory::domain::geometry
{
    template<typename T, typename U>
    struct Point;

    template<typename T, std::size_t N>
    struct Point<T, std::integral_constant<T, N>>
    {};

    template<typename T>
    struct Point<T, std::integral_constant<T, 2>>
    {
        T x = {};
        T y = {};

        Point(T x, T y):
                x(x),
                y(y)
        {
        }
    };

    template<typename T>
    struct Point<T, std::integral_constant<T, 3>>: Point<T, std::integral_constant<T, 2>>
    {
        T z = {};

        Point(T x, T y, T z):
                Point<T, std::integral_constant<T, 2>>(x, y),
                z(z)
        {}
    };

    using Point2f = Point<float, std::integral_constant<std::size_t, 2>>;
    using Point3f = Point<float, std::integral_constant<std::size_t, 3>>;
}