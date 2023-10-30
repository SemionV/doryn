#include "dependencies.h"
#include "base/base.h"

using namespace dory::domain::geometry;

float getBethaCoordinate(Point2d w, Point2d a, Point2d b)
{
    return (a.x*w.y - a.y*w.x) / (a.x*b.y - a.y*b.x);
}

float getAlphaCoordinate(Point2d w, Point2d a, Point2d b)
{
    return (w.x - b.x * getBethaCoordinate(w, a, b)) / a.x;
}

Point2d scaleVector(float scalar, Point2d a)
{
    return Point2d(scalar * a.x, scalar * a.y);
}

Point2d sumVector(Point2d a, Point2d b)
{
    return Point2d(a.x + b.x, a.y + b.y);
}

float dotProduct(Point2d a, Point2d b)
{
    return (a.x * b.x) + (a.y * b.y);
}

float length(Point2d a)
{
    return std::sqrt(std::pow(a.x, 2) + std::pow(a.y, 2));
}

Point2d projectVector(Point2d a, Point2d b)
{
    auto aLength = length(a);
    if(aLength)
    {
        auto scalar = dotProduct(a, b) / std::pow(aLength, 2);
        return scaleVector(scalar, a);
    }

    return Point2d(0.0f, 0.0f);
}

float projectionLength(Point2d a, Point2d b)
{
    auto aLength = length(a);
    if(aLength)
    {
        return dotProduct(a, b) / aLength;
    }

    return 0.f;
}

TEST_CASE( "Vector coordinates in a free 2D basis", "[.][linearAlgebra]" ) 
{
    Point2d c(-2.f, 3.f);
    Point2d d(-3.f, 0.f);

    Point2d w(4.f, 4.f);
    Point2d y(4.f, -2.f);

    Point2d u(-2.f, -4.f);

    Point2d w_cd(getAlphaCoordinate(w, c, d), getBethaCoordinate(w, c, d));
    Point2d y_cd(getAlphaCoordinate(y, c, d), getBethaCoordinate(y, c, d));

    std::cout << "w_cd: (" << w_cd.x << ", " << w_cd.y << ");" << std::endl;
    std::cout << "y_cd: (" << y_cd.x << ", " << y_cd.y << ");" << std::endl;

    Point2d w_ij(sumVector(scaleVector(w_cd.x, c), scaleVector(w_cd.y, d)));
    Point2d y_ij(sumVector(scaleVector(y_cd.x, c), scaleVector(y_cd.y, d)));
    std::cout << "w_ij: (" << w_ij.x << ", " << w_ij.y << ");" << std::endl;
    std::cout << "y_ij: (" << y_ij.x << ", " << y_ij.y << ");" << std::endl;

    Point2d u_wy(getAlphaCoordinate(u, w, y), getBethaCoordinate(u, w, y));
    std::cout << "u_wy: (" << u_wy.x << ", " << u_wy.y << ");" << std::endl;
    Point2d u_cd(getAlphaCoordinate(u, c, d), getBethaCoordinate(u, c, d));
    std::cout << "u_cd: (" << u_cd.x << ", " << u_cd.y << ");" << std::endl;
    Point2d u_ij_wy(sumVector(scaleVector(u_wy.x, w), scaleVector(u_wy.y, y)));
    std::cout << "u_ij_wy: (" << u_ij_wy.x << ", " << u_ij_wy.y << ");" << std::endl;
    Point2d u_ij_cd(sumVector(scaleVector(u_cd.x, c), scaleVector(u_cd.y, d)));
    std::cout << "u_ij_cd: (" << u_ij_cd.x << ", " << u_ij_cd.y << ");" << std::endl;

    Point2d u_cd_wy(sumVector(scaleVector(u_wy.x, w_cd), scaleVector(u_wy.y, y_cd)));
    std::cout << "u_cd_wy: (" << u_cd_wy.x << ", " << u_cd_wy.y << ");" << std::endl;

    Point2d u_ij_cd_wy(sumVector(scaleVector(u_cd_wy.x, c), scaleVector(u_cd_wy.y, d)));
    std::cout << "u_ij_cd_wy: (" << u_ij_cd_wy.x << ", " << u_ij_cd_wy.y << ");" << std::endl;

    Point2d u_wy_ij(sumVector(scaleVector(u.x, w), scaleVector(u.y, y)));
    std::cout << "u_wy_ij: (" << u_wy_ij.x << ", " << u_wy_ij.y << ");" << std::endl;
}

TEST_CASE( "Vector projection", "[.][linearAlgebra]" ) 
{
    Point2d a(7.f, 1.f);
    Point2d b(4.f, 3.f);
    Point2d c(1.f, 3.f);

    Point2d b_a = projectVector(a, b);
    Point2d b_c = projectVector(c, b);

    Point2d ps = sumVector(b_a, b_c);
    float lengthP = projectionLength(a, b);
    float length_b_a = length(b_a);

    std::cout << "b_a: (" << b_a.x << ", " << b_a.y << ");" << std::endl;
    std::cout << "b_c: (" << b_c.x << ", " << b_c.y << ");" << std::endl;
    std::cout << "ps: (" << ps.x << ", " << ps.y << ");" << std::endl;
    std::cout << "lengthP: " << lengthP << std::endl;
    std::cout << "length_b_a: " << length_b_a << std::endl;
}
