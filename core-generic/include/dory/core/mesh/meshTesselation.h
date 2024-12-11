#pragma once

#include <optional>
#include "dory/math/geometry.h"

namespace dory::core::mesh
{
    class Triangulator
    {
        template<typename T, auto N>
        static math::geometry::Shape<T, N> tessellate(const math::geometry::Shape<T, N>& shape)
        {
            math::geometry::Shape<T, N> resultShape { shape };

            std::size_t faceId {};
            for(const auto& face : shape)
            {
                if(face.edgeCount() == 4)
                {
                    //bisect quadrilateral polygon

                    auto firstEdgeId = face.getEdge(0);
                    auto secondEdgeId = face.getEdge(1);
                    auto thirdEdgeId = face.getEdge(2);
                    auto fourthEdgeId = face.getEdge(3);
                    math::geometry::Edge& firstEdge = resultShape.getEdge(firstEdgeId);
                    math::geometry::Edge& secondEdge = resultShape.getEdge(secondEdgeId);
                    math::geometry::Edge& thirdEdge = resultShape.getEdge(thirdEdgeId);
                    math::geometry::Edge& fourthEdge = resultShape.getEdge(fourthEdgeId);

                    auto firstTriangleId = resultShape.addFace({});
                    auto secondTriangleId = resultShape.addFace({});
                    math::geometry::Face& firstTriangle = resultShape.getFace(firstTriangleId);
                    math::geometry::Face& secondTriangle = resultShape.getFace(secondTriangleId);

                    firstEdge.replaceFace(faceId, firstTriangleId);
                    secondEdge.replaceFace(faceId, firstTriangleId);
                    thirdEdge.replaceFace(faceId, secondTriangleId);
                    fourthEdge.replaceFace(faceId, secondTriangleId);

                    math::geometry::Corner corner = math::geometry::getCorner(firstEdge, secondEdge);
                    auto bisectEdgeId = resultShape.addEdge(math::geometry::Edge{ corner.right, corner.left, firstTriangleId, secondTriangleId });

                    firstTriangle.addEdge(firstEdgeId);
                    firstTriangle.addEdge(secondEdgeId);
                    firstTriangle.addEdge(bisectEdgeId);

                    secondTriangle.addEdge(thirdEdgeId);
                    secondTriangle.addEdge(fourthEdgeId);
                    secondTriangle.addEdge(bisectEdgeId);
                }

                ++faceId;
            }
        }
    };
}