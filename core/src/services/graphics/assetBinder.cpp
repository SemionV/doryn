#include <dory/core/registry.h>
#include <dory/core/services/graphics/assetBinder.h>
#include <spdlog/fmt/fmt.h>

namespace dory::core::services::graphics
{
    using namespace resources;
    using namespace resources::bindings;
    using namespace resources::entities;
    using namespace resources::assets;
    using namespace repositories;
    using namespace repositories::bindings;
    using namespace repositories::assets;
    using namespace services;
    using namespace services::graphics;

    AssetBinder::AssetBinder(Registry& registry):
            _registry(registry)
    {}

    void AssetBinder::bindMesh(resources::IdType meshId, resources::entities::GraphicalContext& graphicalContext)
    {
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
                bindMeshData(graphicalContext, mesh, meshBinding);
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

    template<typename TComponent>
    void writeAttributeData(BufferBinding* buffer, Vectors<TComponent> attribute, std::size_t& offset, MeshBinding* meshBinding, IGpuDriver* gpuDriver)
    {
        auto dataSize = getAttributeDataSize(meshBinding->vertexCount, attribute);
        if(dataSize)
        {
            gpuDriver->writeData(buffer, offset, dataSize, attribute.components.data());
            offset += dataSize;

            const auto& attributeBinding = VertexAttributeBinding {
                    offset,
                    attribute.componentsCount,
                    0,
                    VertexAttributeComponentType<TComponent>::value
            };

            meshBinding->vertexAttributes.emplace_back(attributeBinding);
        }
    }

    template<typename... TAttributes>
    void writeVertexAttributes(BufferBinding* buffer, MeshBinding* meshBinding, IGpuDriver* gpuDriver, const TAttributes&... attributes)
    {
        std::size_t offset = 0;
        (writeAttributeData(buffer, attributes, offset, meshBinding, gpuDriver), ...);
    }

    BufferBinding* bindBuffer(IdType& bufferId, std::size_t bufferSize, IBufferBindingRepository* bufferBindingRepository, IGpuDriver* gpuDriver)
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
            bufferBinding = bufferBindingRepository->insert(BufferBinding{});
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

    void AssetBinder::bindMeshData(GraphicalContext& graphicalContext, const Mesh* mesh, MeshBinding* meshBinding)
    {
        auto bufferBindingRepository = _registry.get<IBufferBindingRepository>(graphicalContext.graphicalSystem);
        auto gpuDriver = _registry.get<IGpuDriver>(graphicalContext.graphicalSystem);

        if(bufferBindingRepository && gpuDriver)
        {
            auto bufferBindingRepositoryPtr = (*bufferBindingRepository).get();
            auto gpuDriverPtr = (*gpuDriver).get();

            meshBinding->vertexCount = mesh->vertexCount;
            meshBinding->vertexBufferOffset = 0;
            meshBinding->indexBufferOffset = 0;

            const std::size_t vertexBufferSize = calculateVertexBufferSize(mesh->vertexCount, mesh->positions, mesh->normals, mesh->textureCoordinates, mesh->colors);
            BufferBinding* vertexBufferBinding = bindBuffer(meshBinding->vertexBufferId, vertexBufferSize, bufferBindingRepositoryPtr, gpuDriverPtr);
            if(vertexBufferBinding)
            {
                writeVertexAttributes(vertexBufferBinding, meshBinding, gpuDriverPtr, mesh->positions, mesh->normals, mesh->textureCoordinates, mesh->colors);
            }
            else
            {
                _registry.get<ILogService>([mesh](ILogService* logger)
                                           {
                                               logger->error(fmt::format("Cannot allocate buffer, meshId={0}", mesh->id));
                                           });
            }

            if(!mesh->indices.empty())
            {
                const std::size_t indexBufferSize = mesh->indices.size() * sizeof(Mesh::IndexType);
                BufferBinding* indexBufferBinding = bindBuffer(meshBinding->indexBufferId, indexBufferSize, bufferBindingRepositoryPtr, gpuDriverPtr);
                if(indexBufferBinding)
                {
                    gpuDriver->writeData(indexBufferBinding, 0, indexBufferSize, mesh->indices.data());
                }
            }
        }
    }
}