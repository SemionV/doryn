#include <dory/core/registry.h>
#include <dory/core/services/graphics/meshAssetBinder.h>
#include <spdlog/fmt/fmt.h>

namespace dory::core::services::graphics
{
    using namespace dory;
    using namespace dory::core;
    using namespace dory::core::devices;
    using namespace dory::core::services;
    using namespace dory::core::services::graphics;
    using namespace dory::core::repositories;
    using namespace dory::core::repositories::assets;
    using namespace dory::core::repositories::bindings;
    using namespace dory::core::resources;
    using namespace dory::core::resources::bindings;
    using namespace dory::core::resources::assets;
    using namespace dory::core::resources::entities;

    MeshAssetBinder::MeshAssetBinder(Registry& registry):
            _registry(registry)
    {}

    void MeshAssetBinder::bind(resources::IdType meshId, resources::entities::GraphicalContext& graphicalContext)
    {
        assert(!graphicalContext.meshBindings.contains(meshId)); //binding the previously bound asset

        auto meshBindingRepository = _registry.get<IMeshBindingRepository>(graphicalContext.graphicalSystem);
        auto meshRepository = _registry.get<IMeshRepository>();

        if(meshBindingRepository && meshRepository)
        {
            MeshBinding* meshBinding;
            const auto mesh = meshRepository->get(meshId);

            if(graphicalContext.meshBindings.contains(meshId))
            {
                meshBinding = meshBindingRepository->get(graphicalContext.meshBindings[meshId]);
            }
            else
            {
                meshBinding = meshBindingRepository->insert(MeshBinding{ {}, meshId });
            }

            if(meshBinding && mesh)
            {
                meshBinding->meshId = mesh->id;
                uploadMeshData(graphicalContext, mesh, meshBinding, mesh->positions, mesh->normals,
                               mesh->textureCoordinates, mesh->colors);

                graphicalContext.meshBindings[meshId] = meshBinding->id;
            }
        }
    }

    template<typename TAttribute>
    std::size_t getAttributeDataSize(const std::size_t verticesCount, const TAttribute& vertexAttribute)
    {
        return verticesCount * vertexAttribute.componentsCount * sizeof(typename TAttribute::ComponentsType);
    }

    template<typename... TAttributes>
    std::size_t calculateVertexBufferSize(const std::size_t vertexCount, const TAttributes&... attributes)
    {
        return (getAttributeDataSize(vertexCount, attributes) + ...);
    }

    template<typename T>
    struct VertexAttributeComponentType;

    template<>
    struct VertexAttributeComponentType<float>
    {
        static constexpr auto value = ComponentType::floatType;
    };

    template<>
    struct VertexAttributeComponentType<double>
    {
        static constexpr auto value = ComponentType::doubleType;
    };

    template<>
    struct VertexAttributeComponentType<unsigned int>
    {
        static constexpr auto value = ComponentType::uintType;
    };

    template<typename TComponent>
    void writeAttributeData(BufferBinding* buffer, const Vectors<TComponent>& attribute, std::size_t& offset, MeshBinding* meshBinding, IGpuDevice* gpuDriver)
    {
        auto dataSize = getAttributeDataSize(meshBinding->vertexCount, attribute);
        if(dataSize)
        {
            gpuDriver->writeData(buffer, offset, dataSize, attribute.components.data());

            const auto& attributeBinding = VertexAttributeBinding {
                    offset,
                    attribute.componentsCount,
                    0,
                    VertexAttributeComponentType<TComponent>::value
            };

            meshBinding->vertexAttributes.emplace_back(attributeBinding);
            offset += dataSize;
        }
    }

    template<typename... TAttributes>
    void writeVertexAttributes(BufferBinding* buffer, MeshBinding* meshBinding, IGpuDevice* gpuDriver, const TAttributes&... attributes)
    {
        std::size_t offset = 0;
        (writeAttributeData(buffer, attributes, offset, meshBinding, gpuDriver), ...);
    }

    BufferBinding* bindBuffer(IdType& bufferId, std::size_t bufferSize, IBufferBindingRepository* bufferBindingRepository, IGpuDevice* gpuDriver)
    {
        BufferBinding* bufferBinding = nullptr;
        if(bufferId != nullId)
        {
            bufferBinding = bufferBindingRepository->get(bufferId);
            if(bufferBinding != nullptr)
            {
                gpuDriver->deallocateBuffer(bufferBinding);
            }
        }

        if(!bufferBinding)
        {
            bufferBinding = bufferBindingRepository->insert(BufferBinding{ {}, bufferSize });
        }

        if(bufferBinding)
        {
            if(!gpuDriver->allocateBuffer(bufferBinding, bufferSize))
            {
                bufferBindingRepository->remove(bufferBinding->id);
                return nullptr;
            }

            bufferId = bufferBinding->id;
        }

        return bufferBinding;
    }

    template<typename... TComponents>
    void MeshAssetBinder::uploadMeshData(resources::entities::GraphicalContext& graphicalContext, const resources::assets::Mesh* mesh, resources::bindings::MeshBinding* meshBinding, const Vectors<TComponents>&... vertexAttributes)
    {
        auto bufferBindingRepository = _registry.get<IBufferBindingRepository>(graphicalContext.graphicalSystem);
        auto gpuDriver = _registry.get<IGpuDevice>(graphicalContext.graphicalSystem);

        if(bufferBindingRepository && gpuDriver)
        {
            auto bufferBindingRepositoryPtr = (*bufferBindingRepository).get();
            auto gpuDriverPtr = (*gpuDriver).get();

            meshBinding->vertexCount = mesh->vertexCount;
            meshBinding->vertexBufferOffset = 0;
            meshBinding->indexBufferOffset = 0;
            meshBinding->indexType = VertexAttributeComponentType<Mesh::IndexType>::value;

            const std::size_t vertexBufferSize = calculateVertexBufferSize(mesh->vertexCount, vertexAttributes...);
            BufferBinding* vertexBufferBinding = bindBuffer(meshBinding->vertexBufferId, vertexBufferSize, bufferBindingRepositoryPtr, gpuDriverPtr);
            if(vertexBufferBinding)
            {
                writeVertexAttributes(vertexBufferBinding, meshBinding, gpuDriverPtr, vertexAttributes...);
            }
            else
            {
                _registry.get<ILogService>([mesh](ILogService* logger) {
                    logger->error(fmt::format("Cannot allocate buffer, meshId={0}", mesh->id));
                });
            }

            BufferBinding* indexBufferBinding = nullptr;
            if(!mesh->indices.empty())
            {
                meshBinding->indexCount = mesh->indices.size();

                const std::size_t indexBufferSize = mesh->indices.size() * sizeof(Mesh::IndexType);
                indexBufferBinding = bindBuffer(meshBinding->indexBufferId, indexBufferSize, bufferBindingRepositoryPtr, gpuDriverPtr);
                if(indexBufferBinding)
                {
                    gpuDriver->writeData(indexBufferBinding, 0, indexBufferSize, mesh->indices.data());
                }
            }

            gpuDriver->bindMesh(meshBinding, vertexBufferBinding, indexBufferBinding);
        }
    }
}