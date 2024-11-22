#pragma once

#include <dory/core/resources/entity.h>
#include "bufferBinding.h"

namespace dory::core::resources::bindings
{
    enum class VertexAttributeComponentType
    {
        floatType,
        doubleType
    };

    struct VertexAttributeBinding
    {
        std::size_t offset;
        std::size_t componentsCount;
        VertexAttributeComponentType componentType;
    };

    struct MeshBinding: Entity<>
    {
        IdType meshId;
        IdType vertexBufferId;
        std::size_t vertexBufferOffset;
        std::size_t verticesCount;
        std::vector<VertexAttributeBinding> vertexAttributes;

        IdType indexBufferId;
        std::size_t indexBufferOffset;
        std::size_t indicesCount;
    };
}
