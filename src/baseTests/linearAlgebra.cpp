#include "dependencies.h"

using namespace dory::math;

float getBethaCoordinate(Vector2f w, Vector2f a, Vector2f b)
{
    return (a.x*w.y - a.y*w.x) / (a.x*b.y - a.y*b.x);
}

float getAlphaCoordinate(Vector2f w, Vector2f a, Vector2f b)
{
    return (w.x - b.x * getBethaCoordinate(w, a, b)) / a.x;
}

Vector2f scaleVector(float scalar, Vector2f a)
{
    return {scalar * a.x, scalar * a.y};
}

Vector2f sumVector(Vector2f a, Vector2f b)
{
    return {a.x + b.x, a.y + b.y};
}

float dotProduct(Vector2f a, Vector2f b)
{
    return (a.x * b.x) + (a.y * b.y);
}

double length(Vector2f a)
{
    return std::sqrt(std::pow(a.x, 2) + std::pow(a.y, 2));
}

Vector2f projectVector(Vector2f a, Vector2f b)
{
    auto aLength = length(a);
    if(aLength > 0)
    {
        auto scalar = dotProduct(a, b) / std::pow(aLength, 2);
        return scaleVector((float)scalar, a);
    }

    return {0.0f, 0.0f};
}

double projectionLength(Vector2f a, Vector2f b)
{
    auto aLength = length(a);
    if(aLength > 0)
    {
        return dotProduct(a, b) / aLength;
    }

    return 0.f;
}

TEST_CASE( "Vector coordinates in a free 2D basis", "[.][linearAlgebra]" ) 
{
    Vector2f c(-2.f, 3.f);
    Vector2f d(-3.f, 0.f);

    Vector2f w(4.f, 4.f);
    Vector2f y(4.f, -2.f);

    Vector2f u(-2.f, -4.f);

    Vector2f w_cd(getAlphaCoordinate(w, c, d), getBethaCoordinate(w, c, d));
    Vector2f y_cd(getAlphaCoordinate(y, c, d), getBethaCoordinate(y, c, d));

    std::cout << "w_cd: (" << w_cd.x << ", " << w_cd.y << ");" << std::endl;
    std::cout << "y_cd: (" << y_cd.x << ", " << y_cd.y << ");" << std::endl;

    Vector2f w_ij(sumVector(scaleVector(w_cd.x, c), scaleVector(w_cd.y, d)));
    Vector2f y_ij(sumVector(scaleVector(y_cd.x, c), scaleVector(y_cd.y, d)));
    std::cout << "w_ij: (" << w_ij.x << ", " << w_ij.y << ");" << std::endl;
    std::cout << "y_ij: (" << y_ij.x << ", " << y_ij.y << ");" << std::endl;

    Vector2f u_wy(getAlphaCoordinate(u, w, y), getBethaCoordinate(u, w, y));
    std::cout << "u_wy: (" << u_wy.x << ", " << u_wy.y << ");" << std::endl;
    Vector2f u_cd(getAlphaCoordinate(u, c, d), getBethaCoordinate(u, c, d));
    std::cout << "u_cd: (" << u_cd.x << ", " << u_cd.y << ");" << std::endl;
    Vector2f u_ij_wy(sumVector(scaleVector(u_wy.x, w), scaleVector(u_wy.y, y)));
    std::cout << "u_ij_wy: (" << u_ij_wy.x << ", " << u_ij_wy.y << ");" << std::endl;
    Vector2f u_ij_cd(sumVector(scaleVector(u_cd.x, c), scaleVector(u_cd.y, d)));
    std::cout << "u_ij_cd: (" << u_ij_cd.x << ", " << u_ij_cd.y << ");" << std::endl;

    Vector2f u_cd_wy(sumVector(scaleVector(u_wy.x, w_cd), scaleVector(u_wy.y, y_cd)));
    std::cout << "u_cd_wy: (" << u_cd_wy.x << ", " << u_cd_wy.y << ");" << std::endl;

    Vector2f u_ij_cd_wy(sumVector(scaleVector(u_cd_wy.x, c), scaleVector(u_cd_wy.y, d)));
    std::cout << "u_ij_cd_wy: (" << u_ij_cd_wy.x << ", " << u_ij_cd_wy.y << ");" << std::endl;

    Vector2f u_wy_ij(sumVector(scaleVector(u.x, w), scaleVector(u.y, y)));
    std::cout << "u_wy_ij: (" << u_wy_ij.x << ", " << u_wy_ij.y << ");" << std::endl;
}

TEST_CASE( "Vector projection", "[.][linearAlgebra]" ) 
{
    Vector2f a(7.f, 1.f);
    Vector2f b(4.f, 3.f);
    Vector2f c(1.f, 3.f);

    Vector2f b_a = projectVector(a, b);
    Vector2f b_c = projectVector(c, b);

    Vector2f ps = sumVector(b_a, b_c);
    auto lengthP = projectionLength(a, b);
    auto length_b_a = length(b_a);

    std::cout << "b_a: (" << b_a.x << ", " << b_a.y << ");" << std::endl;
    std::cout << "b_c: (" << b_c.x << ", " << b_c.y << ");" << std::endl;
    std::cout << "ps: (" << ps.x << ", " << ps.y << ");" << std::endl;
    std::cout << "lengthP: " << lengthP << std::endl;
    std::cout << "length_b_a: " << length_b_a << std::endl;
}