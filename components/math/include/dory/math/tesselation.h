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

                    auto firstTriangleResult = resultShape.addFace({});
                    auto secondTriangleResult = resultShape.addFace({});

                    firstEdge.replaceFace(faceId, firstTriangleResult->first);
                    secondEdge.replaceFace(faceId, firstTriangleResult->first);
                    thirdEdge.replaceFace(faceId, secondTriangleResult->first);
                    fourthEdge.replaceFace(faceId, secondTriangleResult->first);

                    math::geometry::Corner corner = math::geometry::getCorner(firstEdge, secondEdge);
                    auto bisectEdgeResult = resultShape.addEdge(math::geometry::Edge{ corner.right, corner.left, firstTriangleResult->first, secondTriangleResult->first });

                    firstTriangleResult->second.addEdge(firstEdgeId);
                    firstTriangleResult->second.addEdge(secondEdgeId);
                    firstTriangleResult->second.addEdge(bisectEdgeResult->first);

                    secondTriangleResult->second.addEdge(thirdEdgeId);
                    secondTriangleResult->second.addEdge(fourthEdgeId);
                    secondTriangleResult->second.addEdge(bisectEdgeResult->first);

                    resultShape.deleteFace(faceId);
                }

                resultShape.deleteFace(faceId);
            }
        }
    };
}