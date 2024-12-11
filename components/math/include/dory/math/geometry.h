#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <deque>
#include <vector>
#include <unordered_map>

namespace dory::math::geometry
{
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
        std::size_t faceA;
        std::size_t faceB;

        bool replaceFace(const std::size_t faceId, const std::size_t newFaceId)
        {
            if(faceA == faceId)
            {
                faceA = newFaceId;
                return true;
            }
            else if(faceB == faceId)
            {
                faceB = faceId;
                return true;
            }

            return false;
        }
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

        [[nodiscard]] std::size_t getEdge(std::size_t edgeIndex) const
        {
            assert(edgeIndex >= 0 && edgeIndex < _edges.size());
            return _edges[edgeIndex];
        }

        [[nodiscard]] std::size_t getLastEdge() const
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

        [[nodiscard]] std::vector<Face>::const_iterator begin() const
        {
            return _faces.begin();
        }

        [[nodiscard]] std::vector<Face>::const_iterator end() const
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
    using Shape3df = Shape3d<float>;

    struct Corner
    {
        std::size_t left {};
        std::size_t center {};
        std::size_t right {};
    };

    Corner getCorner(const Edge& edge1, const Edge& edge2);
    glm::vec3 getNormal(const Face& face, const Shape3df& shape);
}
