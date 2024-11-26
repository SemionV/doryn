#pragma once

#include <dory/core/resources/entity.h>

namespace dory::core::resources::bindings
{
    enum class ComponentType
    {
        floatType,
        doubleType,
        uintType
    };

    struct VertexAttributeBinding
    {
        std::size_t offset;
        std::size_t componentsCount;
        std::size_t stride;
        ComponentType componentType;
        bool normalized {};
    };

    struct MeshBinding: Entity<>
    {
        IdType meshId;
        IdType vertexBufferId;
        std::size_t vertexBufferOffset {};
        std::size_t vertexCount;
        std::vector<VertexAttributeBinding> vertexAttributes;

        IdType indexBufferId;
        std::size_t indexBufferOffset {};
        std::size_t indexCount;
        ComponentType indexType {};
    };
}
