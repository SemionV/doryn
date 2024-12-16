#pragma once

#include <optional>
#include <unordered_map>
#include "dory/math/geometry.h"

namespace dory::core::mesh
{
    struct Triangulator
    {
        template<typename T, auto N>
        static std::vector<std::size_t> tessellatePolygon(const math::geometry::Face& face,  math::geometry::Shape<T, N>& shape)
        {
            if(face.edgeCount() > 4 || face.edgeCount() < 3)
            {
                throw std::exception{};
            }
            if(face.edgeCount() == 4) //bisect quadrilateral polygon
            {
                std::vector<std::size_t> generatedFaces { 2 }; //2 faces(triangles) will be generated

                auto firstEdgeId = face.getEdge(0);
                auto secondEdgeId = face.getEdge(1);
                auto thirdEdgeId = face.getEdge(2);
                auto fourthEdgeId = face.getEdge(3);
                const math::geometry::Edge& firstEdge = shape.getEdge(firstEdgeId);
                const math::geometry::Edge& secondEdge = shape.getEdge(secondEdgeId);

                auto& firstTriangle = shape.addFace();
                auto& secondTriangle = shape.addFace();

                generatedFaces.emplace_back(firstTriangle.id);
                generatedFaces.emplace_back(secondTriangle.id);

                math::geometry::Corner corner = math::geometry::getCorner(firstEdge, secondEdge);
                auto& bisectEdge = shape.addEdge(corner.right, corner.left);

                firstTriangle.addEdge(firstEdgeId);
                firstTriangle.addEdge(secondEdgeId);
                firstTriangle.addEdge(bisectEdge.id);

                secondTriangle.addEdge(thirdEdgeId);
                secondTriangle.addEdge(fourthEdgeId);
                secondTriangle.addEdge(bisectEdge.id);

                return generatedFaces;
            }

            return {};
        }

        template<typename T, auto N>
        static math::geometry::Shape<T, N> tessellateSmooth(const math::geometry::Shape<T, N>& shape)
        {
            math::geometry::Shape<T, N> resultShape { shape };

            for(const auto&[faceId, face] : shape)
            {
                tessellatePolygon(face, resultShape);
                resultShape.deleteFace(faceId);
            }

            return resultShape;
        }

        template<typename T, auto N>
        static math::geometry::Shape<T, N> tessellateFaceted(const math::geometry::Shape<T, N>& shape)
        {
            math::geometry::Shape<T, N> resultShape {};

            for(const auto&[faceId, face] : shape)
            {
                auto& newFace = resultShape.addFace();

                std::unordered_map<std::size_t, std::size_t> verticesMap;
                for(const auto edgeId : face)
                {
                    auto& edge = shape.getEdge(edgeId);
                    std::size_t beginPointId {};
                    std::size_t endPointId {};

                    if(verticesMap.contains(edge.begin))
                    {
                        beginPointId = verticesMap[edge.begin];
                    }
                    else
                    {
                        auto& beginPoint = resultShape.addPoint(shape.getPoint(edge.begin));
                        beginPointId = beginPoint.id;
                        verticesMap[edge.begin] = beginPoint.id;
                    }

                    if(verticesMap.contains(edge.end))
                    {
                        endPointId = verticesMap[edge.end];
                    }
                    else
                    {
                        auto& endPoint = resultShape.addPoint(shape.getPoint(edge.end));
                        endPointId = endPoint.id;
                        verticesMap[edge.end] = endPoint.id;
                    }

                    auto& newEdge = resultShape.addEdge(beginPointId, endPointId);
                    newFace.addEdge(newEdge.id);
                }

                auto newFaces = tessellatePolygon(newFace, resultShape);
                if(newFaces.size() > 0)
                {
                    resultShape.deleteFace(newFace.id);
                }
            }

            return resultShape;
        }
    };
}