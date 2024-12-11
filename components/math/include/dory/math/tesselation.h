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

            for(const auto&[faceId, face] : shape)
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

                    auto& firstTriangle = resultShape.addFace();
                    auto& secondTriangle = resultShape.addFace();

                    firstEdge.replaceFace(faceId, firstTriangle.id);
                    secondEdge.replaceFace(faceId, firstTriangle.id);
                    thirdEdge.replaceFace(faceId, secondTriangle.id);
                    fourthEdge.replaceFace(faceId, secondTriangle.id);

                    math::geometry::Corner corner = math::geometry::getCorner(firstEdge, secondEdge);
                    auto& bisectEdge = resultShape.addEdge(math::geometry::Edge{ corner.right, corner.left, firstTriangle.id, secondTriangle.id });

                    firstTriangle.addEdge(firstEdgeId);
                    firstTriangle.addEdge(secondEdgeId);
                    firstTriangle.addEdge(bisectEdge.id);

                    secondTriangle.addEdge(thirdEdgeId);
                    secondTriangle.addEdge(fourthEdgeId);
                    secondTriangle.addEdge(bisectEdge.id);

                    resultShape.deleteFace(faceId);
                }

                resultShape.deleteFace(faceId);
            }
        }
    };
}