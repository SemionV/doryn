#include <dory/math/geometryMorphing.h>
#include <optional>

namespace dory::math::geometry
{
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

    glm::vec3 getNormal(const Face& face, const Shape3df& shape)
    {
        assert(std::distance(face.begin(), face.end()) > 1);

        auto& edge1 = shape.getEdge(face.getEdge(0));
        auto& edge2 = shape.getEdge(face.getEdge(1));

        Corner corner = getCorner(edge1, edge2);

        auto center = shape.getPoint(corner.center).toVec3();

        auto v1 = center - shape.getPoint(corner.left).toVec3();
        auto v2 = center - shape.getPoint(corner.right).toVec3();

        return glm::cross(v2, v1);
    }

    Shape3df ShapeMorphing::push(const std::size_t faceId, glm::vec3 direction, const Shape3df& shape)
    {
        Shape3df resultShape {shape};

        auto baseFace = shape.getFace(faceId);
        auto oppositeFaceResult = resultShape.addFace(Face{});
        Face& oppositeFace = oppositeFaceResult->second;
        std::size_t firstSideFace{};
        std::optional<std::size_t> prevSideFace{};
        std::size_t i = 0;
        for(const auto& edgeId : baseFace)
        {
            const Edge& edge = shape.getEdge(edgeId);

            std::size_t pointAd {};
            if(i == 0)
            {
                auto pointA = resultShape.getPoint(edge.begin).toVec3();
                pointAd = resultShape.addPoint(Point3f{ pointA + direction })->first;
            }
            else
            {
                //use end point of the previous edge, which is at index 0(reverse insertion order)
                pointAd = resultShape.getEdge(oppositeFace.getEdge(0)).end;
            }

            std::size_t pointBd {};
            if(i == baseFace.edgeCount() - 1)
            {
                //use begin point of the first edge, which is at the end of the collection
                pointBd = resultShape.getEdge(oppositeFace.getLastEdge()).begin;
            }
            else
            {
                auto pointB = resultShape.getPoint(edge.end).toVec3();
                pointBd = resultShape.addPoint(Point3f{ pointB + direction })->first;
            }

            //insert opposite face edges are in reverse order, to point it's normal in opposite direction to base face
            auto oppositeEdgeResult = resultShape.addEdge({pointAd, pointBd});
            oppositeFace.addEdgeInFront(oppositeEdgeResult->first);

            auto sideFaceResult = resultShape.addFace({});
            Face& sideFace = sideFaceResult->second;
            sideFace.addEdge(edgeId);
            if(prevSideFace)
            {
                //reuse the edge from the previous side-face
                sideFace.addEdge(resultShape.getFace(*prevSideFace).getLastEdge());
            }
            else
            {
                auto newEdgeResult = resultShape.addEdge({ edge.begin, oppositeEdgeResult->second.begin });
                sideFace.addEdge(newEdgeResult->first);
                firstSideFace = sideFaceResult->first;
            }
            sideFace.addEdge(oppositeEdgeResult->first);
            if(i == baseFace.edgeCount() - 1)
            {
                //reuse the edge of the first side-face
                sideFace.addEdge(resultShape.getFace(firstSideFace).getEdge(1));
            }
            else
            {
                auto newEdgeResult = resultShape.addEdge({ oppositeEdgeResult->second.end, edge.end });
                sideFace.addEdge(newEdgeResult->first);
            }
            prevSideFace = sideFaceResult->first;

            ++i;
        }

        return resultShape;
    }
}