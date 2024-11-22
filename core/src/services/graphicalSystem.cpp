#include <dory/core/registry.h>
#include <dory/core/services/graphicalSystem.h>
#include <spdlog/fmt/fmt.h>

namespace dory::core::services
{
    GraphicalSystem::GraphicalSystem(core::Registry& registry):
        _registry(registry)
    {}

    void GraphicalSystem::render(core::resources::DataContext& context, const core::resources::entities::Window& window)
    {
        _registry.get<core::repositories::IViewRepository>([this, &context, &window](core::repositories::IViewRepository* viewRepository) {
            auto viewIds = viewRepository->getWindowViews(window.id);
            auto windowService = _registry.get<core::services::IWindowService>(window.windowSystem);
            auto renderer = _registry.get<core::services::graphics::IRenderer>(window.graphicalSystem);
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

    void GraphicalSystem::bindMesh(const resources::IdType meshId, resources::entities::GraphicalContext& graphicalContext)
    {
        auto meshBindingRepository = _registry.get<repositories::bindings::IMeshBindingRepository>(graphicalContext.graphicalSystem);
        auto meshRepository = _registry.get<repositories::assets::IMeshRepository>();

        if(meshBindingRepository && meshRepository)
        {
            resources::bindings::MeshBinding* meshBinding;
            const auto mesh = meshRepository->get(meshId);

            if(graphicalContext.meshBindings.contains(meshId))
            {
                meshBinding = meshBindingRepository->get(graphicalContext.meshBindings[meshId]);
            }
            else
            {
                meshBinding = meshBindingRepository->insert(resources::bindings::MeshBinding{ {}, meshId });
            }

            if(meshBinding && mesh)
            {
                bindMeshData(graphicalContext, mesh, meshBinding);
            }
        }
    }

    template<typename TAttribute>
    std::size_t getVertexAttributeBufferSize(std::size_t verticesCount, TAttribute& vertexAttribute)
    {
        return verticesCount * vertexAttribute.componentsCount * sizeof(TAttribute::ComponentsType);
    }

    template<typename T>
    struct VertexAttributeComponentType
    {

    };

    template<typename T>
    struct VertexAttributeComponentType<float>
    {
        static constexpr auto value = resources::bindings::VertexAttributeComponentType::floatType;
    };

    template<typename T>
    struct VertexAttributeComponentType<double>
    {
        static constexpr auto value = resources::bindings::VertexAttributeComponentType::doubleType;
    };


    void GraphicalSystem::bindMeshData(resources::entities::GraphicalContext& graphicalContext, const resources::assets::Mesh* mesh, resources::bindings::MeshBinding* meshBinding)
    {
        auto bufferBindingRepository = _registry.get<repositories::bindings::IBufferBindingRepository>(graphicalContext.graphicalSystem);
        auto gpuDriver = _registry.get<services::graphics::IGpuDriver>(graphicalContext.graphicalSystem);

        if(bufferBindingRepository && gpuDriver)
        {
            resources::bindings::BufferBinding* vertexBufferBinding = nullptr;
            if(meshBinding->vertexBufferId != resources::nullId)
            {
                vertexBufferBinding = bufferBindingRepository->get(meshBinding->vertexBufferId);
                if(vertexBufferBinding != nullptr)
                {
                    gpuDriver->deallocateBuffer(vertexBufferBinding);
                }
            }

            if(!vertexBufferBinding)
            {
                vertexBufferBinding = bufferBindingRepository->insert(resources::bindings::BufferBinding{});
            }

            if(vertexBufferBinding)
            {
                meshBinding->vertexBufferId = vertexBufferBinding->id;
                meshBinding->vertexBufferOffset = 0;
                meshBinding->verticesCount = mesh->vertexCount;

                const auto positionsDataSize = getVertexAttributeBufferSize(mesh->vertexCount, mesh->positions);
                const auto normalsDataSize = getVertexAttributeBufferSize(mesh->vertexCount, mesh->normals);
                const auto colorsDataSize = getVertexAttributeBufferSize(mesh->vertexCount, mesh->colors);
                const auto texCoordsDataSize = getVertexAttributeBufferSize(mesh->vertexCount, mesh->textureCoordinates);
                const std::size_t bufferSize = positionsDataSize + normalsDataSize + texCoordsDataSize + colorsDataSize;

                if(!gpuDriver->allocateBuffer(vertexBufferBinding, bufferSize))
                {
                    _registry.get<ILogService>([bufferSize, vertexBufferBinding](ILogService* logger)
                    {
                        logger->error(fmt::format("Cannot allocate buffer, size={0}, bindingId={1}", bufferSize, vertexBufferBinding->id));
                    });
                }
                else
                {
                    std::size_t offset = 0;

                    if(positionsDataSize)
                    {
                        gpuDriver->writeDataAsync(vertexBufferBinding, offset, positionsDataSize, mesh->positions.components.data());
                        offset += positionsDataSize;
                        meshBinding->vertexAttributes.emplace_back(resources::bindings::VertexAttributeBinding{offset, mesh->positions.componentsCount, VertexAttributeComponentType<decltype(mesh->positions)::ComponentsType>::});
                    }

                    if(normalsDataSize)
                    {
                        gpuDriver->writeDataAsync(vertexBufferBinding, offset, normalsDataSize, mesh->normals.components.data());
                        offset += normalsDataSize;
                    }

                    if(colorsDataSize)
                    {
                        gpuDriver->writeDataAsync(vertexBufferBinding, offset, colorsDataSize, mesh->colors.components.data());
                        offset += colorsDataSize;
                    }

                    if(texCoordsDataSize)
                    {
                        gpuDriver->writeDataAsync(vertexBufferBinding, offset, texCoordsDataSize, mesh->textureCoordinates.components.data());
                    }
                }
            }
        }
    }
}