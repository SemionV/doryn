#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <iostream>
#include <deque>
#include <dory/core/resources/assets/mesh.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

using namespace dory;
using namespace dory::core;
using namespace dory::core::resources;
using namespace dory::core::resources::assets;

template<typename T, std::size_t Dimensions>
struct Point;

template<typename T>
struct Point<T, 1>
{
    T x {};

    Point() = default;

    explicit Point(const T& _x): x(_x)
    {}

    explicit Point(const glm::vec1& v): x(v.x)
    {}

    [[nodiscard]] glm::vec1 toVec1() const
    {
        return glm::vec3{ x };
    }
};

using Point1f = Point<float, 1>;

template<typename T>
struct Point<T, 2>: public Point<T, 1>
{
    T y {};

    Point() = default;

    explicit Point(const T& _x, const T& _y):
            Point<T, 1>(_x),
            y(_y)
    {}

    explicit Point(const glm::vec2& v): Point<T, 1>(v), y(v.y)
    {}

    [[nodiscard]] glm::vec2 toVec2() const
    {
        return glm::vec2{ this->x, y };
    }
};

using Point2f = Point<float, 2>;

template<typename T>
struct Point<T, 3>: public Point<T, 2>
{
    T z {};

    Point() = default;

    explicit Point(const T& _x, const T& _y, const T& _z):
            Point<T, 2>(_x, _y),
            z(_z)
    {}

    explicit Point(const glm::vec3& v): Point<T, 2>(v), z(v.z)
    {}

    [[nodiscard]] glm::vec3 toVec3() const
    {
        return glm::vec3{ this->x, this->y, z };
    }
};

using Point3f = Point<float, 3>;

struct Edge
{
    std::size_t begin;
    std::size_t end;
};

struct Face
{
private:
    using EdgesType = std::deque<std::size_t>;

    EdgesType _edges;

public:
    Face() = default;

    explicit Face(std::size_t edgesCount):
            _edges{edgesCount}
    {}

    Face(std::initializer_list<std::size_t> edges):
            _edges{edges}
    {}

    [[nodiscard]] EdgesType::const_iterator begin() const
    {
        return _edges.begin();
    }

    [[nodiscard]] EdgesType::const_iterator end() const
    {
        return _edges.end();
    }

    std::size_t addEdge(std::size_t edge)
    {
        auto edgesCount = _edges.size();
        _edges.push_back(edge);
        return edgesCount;
    }

    std::size_t addEdgeInFront(std::size_t edge)
    {
        auto edgesCount = _edges.size();
        _edges.push_front(edge);
        return edgesCount;
    }

    [[nodiscard]] std::size_t get(std::size_t edgeIndex) const
    {
        assert(edgeIndex >= 0 && edgeIndex < _edges.size());
        return _edges[edgeIndex];
    }

    [[nodiscard]] std::size_t getLast() const
    {
        assert(!_edges.empty());
        return _edges[_edges.size() - 1];
    }

    [[nodiscard]] std::size_t edgeCount() const
    {
        return _edges.size();
    }
};

template<typename T, std::size_t Dimensions>
class Shape
{
private:
    using PointType = Point<T, Dimensions>;

    std::vector<PointType> _points;
    std::vector<Edge> _edges;
    std::vector<Face> _faces;

public:
    static constexpr std::size_t dimensions = Dimensions;

    Shape() = default;

    Shape(std::initializer_list<PointType> points, std::initializer_list<Edge> edges, std::initializer_list<Face> faces):
        _points{points}, _edges{edges}, _faces{faces}
    {}

    std::size_t getPointCount()
    {
        return _points.size();
    }

    std::size_t getEdgeCount()
    {
        return _edges.size();
    }

    std::size_t getFaceCount()
    {
        return _faces.size();
    }

    std::size_t addPoint(const Point<T, Dimensions>& point)
    {
        auto pointCount = getPointCount();
        _points.emplace_back(point);
        return pointCount;
    }

    std::size_t addEdge(const Edge& edge)
    {
        auto edgeCount = getEdgeCount();
        _edges.emplace_back(edge);
        return edgeCount;
    }

    std::size_t addFace(const Face& face)
    {
        auto faceCount = getFaceCount();
        _faces.emplace_back(face);
        return faceCount;
    }

    [[nodiscard]] const PointType& getPoint(std::size_t pointId) const
    {
        assert(pointId < _points.size() && pointId >= 0);

        return _points[pointId];
    }

    [[nodiscard]] const Edge& getEdge(std::size_t edgeId) const
    {
        assert(edgeId < _edges.size()  && edgeId >= 0);

        return _edges[edgeId];
    }

    [[nodiscard]] const Face& getFace(std::size_t faceId) const
    {
        assert(faceId < _faces.size() && faceId >= 0);

        return _faces[faceId];
    }

    Face& getFace(std::size_t faceId)
    {
        assert(faceId < _faces.size() && faceId >= 0);

        return _faces[faceId];
    }

    [[nodiscard]] std::vector<Face>::const_iterator facesBegin() const
    {
        return _faces.begin();
    }

    [[nodiscard]] std::vector<Face>::const_iterator facesEnd() const
    {
        return _faces.end();
    }

    [[nodiscard]] std::size_t facesCount() const
    {
        return _faces.size();
    }
};

template<typename T>
using Shape3d = Shape<T, 3>;

struct Corner
{
    std::size_t left {};
    std::size_t center {};
    std::size_t right {};
};

Corner getCorner(const Edge& edge1, const Edge& edge2)
{
    if(edge1.begin == edge2.begin)
    {
        return Corner {edge1.end, edge1.begin, edge2.end};
    }
    else if(edge1.begin == edge2.end)
    {
        return Corner {edge1.end, edge1.begin, edge2.begin};
    }
    else if(edge1.end == edge2.begin)
    {
        return Corner {edge1.begin, edge1.end, edge2.end};
    }
    else if(edge1.end == edge2.end)
    {
        return Corner {edge1.begin, edge1.end, edge2.begin};
    }

    return {};
}

template<typename T>
glm::vec3 getNormal(const Face& face, const Shape3d<T>& shape)
{
    assert(std::distance(face.begin(), face.end()) > 1);

    auto& edge1 = shape.getEdge(face.get(0));
    auto& edge2 = shape.getEdge(face.get(1));

    Corner corner = getCorner(edge1, edge2);

    auto center = shape.getPoint(corner.center).toVec3();

    auto v1 = center - shape.getPoint(corner.left).toVec3();
    auto v2 = center - shape.getPoint(corner.right).toVec3();

    return glm::cross(v2, v1);
}

template<typename T>
Shape3d<T> push(const std::size_t faceId, glm::vec3 direction, const Shape3d<T>& shape)
{
    Shape3d<T> resultShape {shape};

    auto baseFace = shape.getFace(faceId);
    auto oppositeFaceId = resultShape.addFace(Face{});
    std::size_t firstSideFace{};
    std::optional<std::size_t> prevSideFace{};
    std::size_t i = 0;
    for(const auto& edgeId : baseFace)
    {
        Face& oppositeFace = resultShape.getFace(oppositeFaceId);
        const Edge& edge = shape.getEdge(edgeId);

        std::size_t pointAd {};
        if(i == 0)
        {
            auto pointA = resultShape.getPoint(edge.begin).toVec3();
            pointAd = resultShape.addPoint(Point3f{ pointA + direction });
        }
        else
        {
            //use end point of the previous edge, which is at index 0(reverse insertion order)
            pointAd = resultShape.getEdge(oppositeFace.get(0)).end;
        }

        std::size_t pointBd {};
        if(i == baseFace.edgeCount() - 1)
        {
            //use begin point of the first edge, which is at the end of the collection
            pointBd = resultShape.getEdge(oppositeFace.getLast()).begin;
        }
        else
        {
            auto pointB = resultShape.getPoint(edge.end).toVec3();
            pointBd = resultShape.addPoint(Point3f{ pointB + direction });
        }

        //insert opposite face edges are in reverse order, to point it's normal in opposite direction to base face
        auto oppositeEdgeId = resultShape.addEdge({pointAd, pointBd});
        const Edge& oppositeEdge = resultShape.getEdge(oppositeEdgeId);
        oppositeFace.addEdgeInFront(oppositeEdgeId);

        auto sideFaceId = resultShape.addFace({});
        Face& sideFace = resultShape.getFace(sideFaceId);
        sideFace.addEdge(edgeId);
        if(prevSideFace)
        {
            //reuse the edge from the previous side-face
            sideFace.addEdge(resultShape.getFace(*prevSideFace).getLast());
        }
        else
        {
            auto newEdgeId = resultShape.addEdge({ edge.begin, oppositeEdge.begin });
            sideFace.addEdge(newEdgeId);
            firstSideFace = sideFaceId;
        }
        sideFace.addEdge(oppositeEdgeId);
        if(i == baseFace.edgeCount() - 1)
        {
            //reuse the edge of the first side-face
            sideFace.addEdge(resultShape.getFace(firstSideFace).get(1));
        }
        else
        {
            auto newEdgeId = resultShape.addEdge({ oppositeEdge.end, edge.end });
            sideFace.addEdge(newEdgeId);
        }
        prevSideFace = sideFaceId;

        ++i;
    }

    return resultShape;
}

template<typename T>
Mesh buildMesh(const Shape3d<T>& shape)
{

}

template<typename T, auto N>
void assertPrism(const Shape<T, N> prism)
{
    EXPECT_EQ(prism.points.size() / N, 6);
}

TEST(Geometry, triangularPrism)
{
    auto width = 8.f;

    auto triangularPrismBase = Shape3d<float>{
            { Point3f{ width, -width, 0 }, Point3f{ width, 0, 0 }, Point3f{ 0, 0, 0 } },
            { Edge{ 0, 1 }, Edge{ 1, 2 }, Edge{ 2, 0 }},
            { Face{ 0, 1, 2 } }
    };

    auto& face = triangularPrismBase.getFace(0);
    auto normal = glm::normalize(getNormal(face, triangularPrismBase));
    EXPECT_EQ(normal, glm::vec3(0, 0, 1));

    Shape3d<float> triangularPrism = push(0, math::Vector3f{ 0, 0, -8 }, triangularPrismBase);
    EXPECT_EQ(triangularPrism.facesCount(), 5);
    /*auto& oppositeFace = triangularPrism.faces[1];
    EXPECT_EQ(oppositeFace.size(), 3);
    normal = glm::normalize(getNormal(oppositeFace[0], oppositeFace[1], triangularPrism));
    EXPECT_EQ(normal, glm::vec3(0, 0, -1));

    assertPrism(triangularPrism);*/

    //auto mesh = buildMesh(triangularPrism);
}