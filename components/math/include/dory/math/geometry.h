#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <deque>
#include <vector>
#include <unordered_map>

namespace dory::math::geometry
{
    struct Primitive
    {
        std::size_t id {};

        Primitive() = default;

        explicit Primitive(const std::size_t _id):
            id(_id)
        {}
    };

    template<typename T, std::size_t Dimensions>
    struct Point;

    template<typename T>
    struct Point<T, 1>: public Primitive
    {
        T x {};

        Point() = default;

        explicit Point(const std::size_t _id, const T& _x):
            Primitive(_id),
            x(_x)
        {}

        explicit Point(const std::size_t _id, const glm::vec1& v):
            Primitive(_id),
            x(v.x)
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

        explicit Point(const std::size_t _id, const T& _x, const T& _y):
                Point<T, 1>(_id, _x),
                y(_y)
        {}

        explicit Point(const std::size_t _id, const glm::vec2& v):
            Point<T, 1>(_id, v), y(v.y)
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

        explicit Point(const std::size_t _id, const T& _x, const T& _y, const T& _z):
                Point<T, 2>(_id, _x, _y),
                z(_z)
        {}

        explicit Point(const std::size_t _id, const glm::vec3& v):
            Point<T, 2>(_id, v), z(v.z)
        {}

        [[nodiscard]] glm::vec3 toVec3() const
        {
            return glm::vec3{ this->x, this->y, z };
        }
    };

    using Point3f = Point<float, 3>;

    struct Edge: public Primitive
    {
        constexpr static std::size_t invalidValue {std::numeric_limits<std::size_t>::max()};

        std::size_t begin { invalidValue };
        std::size_t end { invalidValue };

        Edge() = default;

        explicit Edge(const std::size_t id):
            Primitive(id)
        {}

        explicit Edge(const std::size_t id, std::size_t begin, std::size_t end):
            Primitive(id),
            begin(begin),
            end(end)
        {}
    };

    struct Face: public Primitive
    {
    private:
        using EdgesType = std::deque<std::size_t>;

        EdgesType _edges;

    public:
        Face() = default;

        explicit Face(const std::size_t id):
            Primitive(id)
        {}

        explicit Face(std::size_t id, std::size_t edgesCount):
            Primitive(id),
            _edges{edgesCount}
        {}

        Face(std::size_t id, std::initializer_list<std::size_t> edges):
            Primitive(id),
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

        std::size_t addEdge(const std::size_t edge)
        {
            const auto edgesCount = _edges.size();
            _edges.push_back(edge);
            return edgesCount;
        }

        std::size_t addEdgeInFront(const std::size_t edge)
        {
            const auto edgesCount = _edges.size();
            _edges.push_front(edge);
            return edgesCount;
        }

        [[nodiscard]] std::size_t getEdge(const std::size_t edgeIndex) const
        {
            assert(edgeIndex < _edges.size());
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

        std::size_t pointCounter {};
        std::size_t edgeCounter {};
        std::size_t faceCounter {};

        std::unordered_map<std::size_t, PointType> _points;
        std::unordered_map<std::size_t, Edge> _edges;
        std::unordered_map<std::size_t, Face> _faces;

    public:
        static constexpr std::size_t dimensions = Dimensions;

        Shape() = default;

        Shape(const std::unordered_map<std::size_t, PointType>& points,
              const std::unordered_map<std::size_t, Edge>& edges,
              const std::unordered_map<std::size_t, Face>& faces):
                _points{points}, _edges{edges}, _faces{faces}
        {
            pointCounter = points.size();
            edgeCounter = edges.size();
            faceCounter = faces.size();
        }

        [[nodiscard]] std::size_t getPointCount() const
        {
            return _points.size();
        }

        [[nodiscard]] std::size_t getEdgeCount() const
        {
            return _edges.size();
        }

        [[nodiscard]] std::size_t getFaceCount() const
        {
            return _faces.size();
        }

        template<typename... Ts>
        PointType& addPoint(Ts&&... parameters)
        {
            auto id = pointCounter++;
            return _points.emplace(id, PointType{ id, std::forward<Ts>(parameters)... }).first->second;
        }

        PointType& addPoint(const PointType& point)
        {
            auto id = pointCounter++;
            auto& newPoint =  _points.emplace(id, PointType{ point }).first->second;
            newPoint.id = id;

            return newPoint;
        }

        template<typename... Ts>
        Edge& addEdge(Ts&&... parameters)
        {
            auto id = edgeCounter++;
            return _edges.emplace(id, Edge{ id, std::forward<Ts>(parameters)... }).first->second;
        }

        Edge& addEdge(const Edge& edge)
        {
            auto id = edgeCounter++;
            auto& newEdge =  _edges.emplace(id, Edge{edge }).first->second;
            newEdge.id = id;

            return newEdge;
        }

        template<typename... Ts>
        Face& addFace(Ts&&... parameters)
        {
            auto id = faceCounter++;
            return _faces.emplace(id, Face{ id, std::forward<Ts>(parameters)... }).first->second;
        }

        Face& addFace(const Face& face)
        {
            auto id = faceCounter++;
            auto& newFace =  _faces.emplace(id, Face{ face }).first->second;
            newFace.id = id;

            return newFace;
        }

        [[nodiscard]] const PointType& getPoint(std::size_t pointId) const
        {
            assert(_points.contains(pointId));
            return _points.at(pointId);
        }

        [[nodiscard]] const Edge& getEdge(std::size_t edgeId) const
        {
            assert(_edges.contains(edgeId));

            return _edges.at(edgeId);
        }

        [[nodiscard]] const Face& getFace(std::size_t faceId) const
        {
            assert(_edges.contains(faceId));
            return _faces.at(faceId);
        }

        Face& getFace(std::size_t faceId)
        {
            assert(_edges.contains(faceId));
            return _faces[faceId];
        }

        [[nodiscard]] auto begin() const
        {
            return _faces.begin();
        }

        [[nodiscard]] auto end() const
        {
            return _faces.end();
        }

        void deleteFace(const std::size_t faceId)
        {
            _faces.erase(faceId);
        }

        const auto& getPoints() const
        {
            return _points;
        }

        const auto& getEdges() const
        {
            return _edges;
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
