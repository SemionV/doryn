#include <dory/core/registry.h>
#include <dory/core/services/graphicalSystem.h>
#include <spdlog/fmt/fmt.h>

namespace dory::core::services
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

    GraphicalSystem::GraphicalSystem(core::Registry& registry):
        _registry(registry)
    {}

    void GraphicalSystem::render(DataContext& context, const Window& window)
    {
        _registry.get<IViewRepository>([this, &context, &window](IViewRepository* viewRepository) {
            auto viewIds = viewRepository->getWindowViews(window.id);
            auto windowService = _registry.get<IWindowService>(window.windowSystem);
            auto renderer = _registry.get<IRenderer>(window.graphicalSystem);
            for(const auto& viewId : viewIds)
            {
                auto view = viewRepository->get(viewId);

                if(view && windowService && renderer)
                {
                    windowService->setCurrentWindow(window.id);
                    renderer->draw(context, *view);
                    windowService->swapBuffers(window);
                }
            }
        });
    }

    bool GraphicalSystem::uploadProgram(core::resources::entities::ShaderProgram& program,
                                        const core::resources::entities::Window& window)
    {
        auto shaderService = _registry.get<graphics::IShaderService>(window.graphicalSystem);
        auto windowService = _registry.get<core::services::IWindowService>(window.windowSystem);
        auto logger = _registry.get<core::services::ILogService>();

        if(shaderService && windowService)
        {
            windowService->setCurrentWindow(window.id);
            if(shaderService->initializeProgram(program))
            {
                auto shaderRepository = _registry.get<repositories::IShaderRepository>(window.graphicalSystem);
                auto fileService = _registry.get<services::IFileService>();
                if(shaderRepository && fileService)
                {
                    shaderRepository->each([&fileService, &program, &shaderService, &logger](auto& shader) {
                        auto sourceCode = fileService->read(shader.filePath);
                        if(!shaderService->initializeShader(program, shader, sourceCode) && logger)
                        {
                            logger->error("Cannot initialize shader: " + program.key + ", " + shader.filePath.string());
                        }
                    });
                }
            }
            else
            {
                if(logger)
                {
                    logger->error("Cannot initialize shader program: " + program.key);
                }

                return false;
            }
        }

        return true;
    }

    void GraphicalSystem::bindMesh(const IdType meshId, GraphicalContext& graphicalContext)
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
    void writeAttributeData(BufferBinding* buffer, std::size_t vertexCount, Vectors<TComponent> attribute, std::size_t& offset, MeshBinding* meshBinding, IGpuDriver* gpuDriver)
    {
        auto dataSize = getAttributeDataSize(vertexCount, attribute);
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

    BufferBinding* bindVertexBuffer(const Mesh* mesh, MeshBinding* meshBinding, IBufferBindingRepository* bufferBindingRepository, IGpuDriver* gpuDriver)
    {
        BufferBinding* vertexBufferBinding = nullptr;
        if(meshBinding->vertexBufferId != nullId)
        {
            vertexBufferBinding = bufferBindingRepository->get(meshBinding->vertexBufferId);
            if(vertexBufferBinding != nullptr)
            {
                gpuDriver->deallocateBuffer(vertexBufferBinding);
            }
        }

        if(!vertexBufferBinding)
        {
            vertexBufferBinding = bufferBindingRepository->insert(BufferBinding{});
        }

        if(vertexBufferBinding)
        {
            const std::size_t bufferSize = calculateVertexBufferSize(mesh->vertexCount, mesh->positions, mesh->normals, mesh->textureCoordinates, mesh->colors);

            if(!gpuDriver->allocateBuffer(vertexBufferBinding, bufferSize))
            {
                bufferBindingRepository->remove(vertexBufferBinding->id);
                return nullptr;
            }

            meshBinding->vertexBufferId = vertexBufferBinding->id;
            meshBinding->vertexBufferOffset = 0;
            meshBinding->verticesCount = mesh->vertexCount;
        }

        return vertexBufferBinding;
    }

    void GraphicalSystem::bindMeshData(GraphicalContext& graphicalContext, const Mesh* mesh, MeshBinding* meshBinding)
    {
        auto bufferBindingRepository = _registry.get<IBufferBindingRepository>(graphicalContext.graphicalSystem);
        auto gpuDriver = _registry.get<IGpuDriver>(graphicalContext.graphicalSystem);

        if(bufferBindingRepository && gpuDriver)
        {
            BufferBinding* vertexBufferBinding = bindVertexBuffer(mesh, meshBinding, (*bufferBindingRepository).get(), (*gpuDriver).get());
            if(vertexBufferBinding)
            {
                std::size_t offset = 0;

                auto gpuDriverPtr = (*gpuDriver).get();
                writeAttributeData(vertexBufferBinding, mesh->vertexCount, mesh->positions, offset, meshBinding, gpuDriverPtr);
                writeAttributeData(vertexBufferBinding, mesh->vertexCount, mesh->normals, offset, meshBinding, gpuDriverPtr);
                writeAttributeData(vertexBufferBinding, mesh->vertexCount, mesh->textureCoordinates, offset, meshBinding, gpuDriverPtr);
                writeAttributeData(vertexBufferBinding, mesh->vertexCount, mesh->colors, offset, meshBinding, gpuDriverPtr);
            }
            else
            {
                _registry.get<ILogService>([mesh](ILogService* logger)
                {
                    logger->error(fmt::format("Cannot allocate buffer, meshId={0}", mesh->id));
                });
            }
        }
    }
}