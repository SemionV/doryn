#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <dory/core/registry.h>
#include <dory/core/services/graphics/assetBinder.h>
#include "dory/core/devices/iGpuDevice.h"
#include <dory/core/repositories/assets/iMeshRepository.h>
#include <dory/core/repositories/bindings/iMeshBindingRepository.h>
#include <dory/core/repositories/bindings/iBufferBindingRepository.h>

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
using namespace dory::core::resources::objects;
using namespace testing;

class GpuDeviceMock: public IGpuDevice
{
public:
    MOCK_METHOD(void, connect, (DataContext& context));
    MOCK_METHOD(void, disconnect, (DataContext& context));
    MOCK_METHOD(bool, allocateBuffer, (BufferBinding* bufferBinding, std::size_t size));
    MOCK_METHOD(void, deallocateBuffer, (BufferBinding* bufferBinding));
    MOCK_METHOD(void, writeData, (BufferBinding* bufferBinding, std::size_t offset, std::size_t size, const void* data));
    MOCK_METHOD(void, bindMesh, (MeshBinding* meshBinding, const resources::bindings::BufferBinding* vertexBuffer, const resources::bindings::BufferBinding* indexBuffer));

    ;
    MOCK_METHOD(void, drawFrame, (const Frame& frame));
};

template<typename TInterface, typename TEntity = TInterface::EntityType>
class EntityRepositoryMock: public TInterface
{
protected:
    MOCK_METHOD(void, setId, (TEntity& entity));

public:
    MOCK_METHOD(std::size_t, count, ());
    MOCK_METHOD(TEntity*, get, (typename TEntity::IdType id));
    MOCK_METHOD(void, store, (TEntity& entity));
    MOCK_METHOD(TEntity*, insert, (TEntity&& entity));
    MOCK_METHOD(typename TEntity::IdType, insert, (const TEntity& entity));
    MOCK_METHOD(void, remove, (typename TEntity::IdType id));
    MOCK_METHOD(TEntity*, scan, (std::function<bool(TEntity& entity)> predicate));
    MOCK_METHOD(void, each, (std::function<void(TEntity& entity)> predicate));
};

class MeshRepository: public EntityRepositoryMock<IMeshRepository>
{};

class MeshBindingRepository: public EntityRepositoryMock<IMeshBindingRepository>
{};

class BufferBindingRepository: public EntityRepositoryMock<IBufferBindingRepository>
{};

template<typename TComponent>
std::size_t getVertexAttributeSize(const Vectors<TComponent>& attribute)
{
    return sizeof(TComponent) * attribute.components.size();
}

template<typename... TComponents>
std::size_t getVertexAttributesSize(const Vectors<TComponents>&... attributes)
{
    return (getVertexAttributeSize(attributes) + ...);
}

template<typename TComponents>
void assertVertexAttribute(const std::vector<VertexAttributeBinding>& attributeBindings, std::size_t index, const Vectors<TComponents>& attribute, std::size_t& offset, const ComponentType expectedCompoenentType)
{
    auto& attributeBinding = attributeBindings[index];
    EXPECT_EQ(attributeBinding.offset, offset);
    EXPECT_EQ(attributeBinding.componentsCount, attribute.componentsCount);
    EXPECT_EQ(attributeBinding.componentType, expectedCompoenentType);
    EXPECT_EQ(attributeBinding.stride, 0);
    offset += sizeof(TComponents) * attribute.components.size();
}

void assertVertexAttribute(const std::vector<VertexAttributeBinding>& attributeBindings, std::size_t index, const Vectors<float>& attribute, std::size_t& offset)
{
    assertVertexAttribute(attributeBindings, index, attribute, offset, ComponentType::floatType);
}

void assertVertexAttribute(const std::vector<VertexAttributeBinding>& attributeBindings, std::size_t index, const Vectors<double>& attribute, std::size_t& offset)
{
    assertVertexAttribute(attributeBindings, index, attribute, offset, ComponentType::doubleType);
}

template<typename... TComponents, std::size_t... Indices>
void assertVertexAttributesHelper(std::vector<VertexAttributeBinding>& attributeBindings, std::index_sequence<Indices...>, const Vectors<TComponents>&... attributes)
{
    std::size_t offset = 0;
    (assertVertexAttribute(attributeBindings, Indices, attributes, offset), ...);
}

template<typename... TComponents>
void assertVertexAttributes(std::vector<VertexAttributeBinding>& attributeBindings, const Vectors<TComponents>&... attributes)
{
    EXPECT_EQ(attributeBindings.size(), sizeof...(TComponents));
    assertVertexAttributesHelper(attributeBindings, std::make_index_sequence<sizeof...(TComponents)>{}, attributes...);
}

template<typename... TComponents>
void expectDataWrites(GpuDeviceMock* gpuDriver, const BufferBinding& buffer, const Vectors<TComponents>&... attributes)
{
    std::size_t offset = 0;
    (expectDataWrite(gpuDriver, buffer, offset, attributes), ...);
}

template<typename TComponent>
void expectDataWrite(GpuDeviceMock* gpuDriver, const BufferBinding& buffer, std::size_t& offset, const Vectors<TComponent>& attribute)
{
    auto bufferMatcher = Pointee(Field(&BufferBinding::id, Eq(buffer.id)));
    std::size_t attributeDataSize = getVertexAttributeSize(attribute);
    EXPECT_CALL(*gpuDriver, writeData(bufferMatcher, Eq(offset), Eq(attributeDataSize), Eq(attribute.components.data()))).WillOnce(Return());
    offset += attributeDataSize;
}

template<typename... TComponents>
void assertMeshBinding(Mesh& mesh, const Vectors<TComponents>&... vertexAttributes)
{
    constexpr auto graphicalSystemType = GraphicalSystem::opengl;
    Registry registry{};
    dory::generic::extension::LibraryHandle libraryHandle {};
    GraphicalContext graphicalContext = {{}, graphicalSystemType};

    const auto meshRepository = std::make_shared<MeshRepository>();
    registry.set<IMeshRepository>(libraryHandle, meshRepository);

    const auto meshBindingRepository = std::make_shared<MeshBindingRepository>();
    registry.set<IMeshBindingRepository, graphicalSystemType>(libraryHandle, meshBindingRepository);

    const auto bufferBindingRepository = std::make_shared<BufferBindingRepository>();
    registry.set<IBufferBindingRepository, graphicalSystemType>(libraryHandle, bufferBindingRepository);

    const auto gpuDriver = std::make_shared<GpuDeviceMock>();
    registry.set<IGpuDevice, graphicalSystemType>(libraryHandle, gpuDriver);

    auto assetBinder = AssetBinder(registry);

    auto meshBinding = MeshBinding{ 1 };
    auto vertexBufferBinding = BufferBinding { 1 };
    auto indexBufferBinding = BufferBinding { 2 };
    std::size_t vertexBufferSize = getVertexAttributesSize(vertexAttributes...);
    std::size_t indexBufferSize = mesh.indices.size() * sizeof(Mesh::IndexType);

    EXPECT_CALL(*meshRepository, get(_)).WillOnce(Return(&mesh));

    auto insertMeshBindingMatcher = Matcher<MeshBinding&&>(Field(&MeshBinding::meshId, Eq(mesh.id)));
    EXPECT_CALL(*meshBindingRepository, insert(insertMeshBindingMatcher)).WillOnce(Return(&meshBinding));

    auto insertVertexBufferBindingMatcher = Matcher<BufferBinding&&>(Field(&BufferBinding::size, Eq(vertexBufferSize)));
    EXPECT_CALL(*bufferBindingRepository, insert(insertVertexBufferBindingMatcher))
            .WillOnce(WithArg<0>([&vertexBufferBinding](BufferBinding&& buffer) {
                vertexBufferBinding.size = buffer.size;
                return &vertexBufferBinding;
            }));

    auto insertIndexBufferBindingMatcher = Matcher<BufferBinding&&>(Field(&BufferBinding::size, Eq(indexBufferSize)));
    EXPECT_CALL(*bufferBindingRepository, insert(insertIndexBufferBindingMatcher))
            .WillOnce(WithArg<0>([&indexBufferBinding](BufferBinding&& buffer) {
                indexBufferBinding.size = buffer.size;
                return &indexBufferBinding;
            }));

    auto meshMatcher = Pointee(Field(&Mesh::id, Eq(mesh.id)));
    auto vertexBufferMatcher = Pointee(Field(&BufferBinding::id, Eq(vertexBufferBinding.id)));
    auto indexBufferMatcher = Pointee(Field(&BufferBinding::id, Eq(indexBufferBinding.id)));

    EXPECT_CALL(*gpuDriver, allocateBuffer(vertexBufferMatcher, Eq(vertexBufferSize))).WillOnce(Return(true));
    EXPECT_CALL(*gpuDriver, allocateBuffer(indexBufferMatcher, Eq(indexBufferSize))).WillOnce(Return(true));

    expectDataWrites(gpuDriver.get(), vertexBufferBinding, vertexAttributes...);

    EXPECT_CALL(*gpuDriver, writeData(indexBufferMatcher, Eq(0), Eq(indexBufferSize), Eq(mesh.indices.data()))).WillOnce(Return());

    EXPECT_CALL(*gpuDriver, bindMesh(meshMatcher, vertexBufferMatcher, indexBufferMatcher)).WillOnce(Return());

    assetBinder.bindMesh(mesh.id, graphicalContext);

    EXPECT_EQ(mesh.id, meshBinding.meshId);
    EXPECT_EQ(mesh.vertexCount, meshBinding.vertexCount);
    EXPECT_EQ(0, meshBinding.vertexBufferOffset);
    EXPECT_EQ(vertexBufferBinding.id, meshBinding.vertexBufferId);
    EXPECT_EQ(mesh.indices.size(), meshBinding.indexCount);
    EXPECT_EQ(0, meshBinding.indexBufferOffset);
    EXPECT_EQ(ComponentType::uintType, meshBinding.indexType);
    EXPECT_EQ(indexBufferBinding.id, meshBinding.indexBufferId);
    EXPECT_EQ(vertexBufferSize, vertexBufferBinding.size);
    EXPECT_EQ(indexBufferSize, indexBufferBinding.size);
    EXPECT_TRUE(graphicalContext.meshBindings.contains(mesh.id));
    EXPECT_EQ(graphicalContext.meshBindings[mesh.id], meshBinding.id);

    assertVertexAttributes(meshBinding.vertexAttributes, vertexAttributes...);
}

TEST(AssetBinder, bindMesh)
{
    auto mesh = Mesh
    {
        1,
        3,
        Vectors<float>{ 2, std::vector<float>{ 1.f, 1.f,  -1.f, 1.f,  0.f, -1.f } },
        Vectors<float>{ 3, std::vector<float>{ 0.f, 0.f, 1.f,  0.f, 0.f, 1.f,  0.f, 0.f, 1.f } },
        Vectors<float>{ 2, std::vector<float>{ 1.f, 0.f,  0.f, 0.f,  0.5f, 1.f } },
        Vectors<float>{ 3, std::vector<float>{ 1.f, 0.f, 0.f,  0.f, 1.f, 0.f,  0.f, 0.f, 1.f } },
        std::vector<Mesh::IndexType >{ 0, 1, 2 }
    };

    assertMeshBinding(mesh, mesh.positions, mesh.normals, mesh.textureCoordinates, mesh.colors);
}

TEST(AssetBinder, bindMeshWithFewerAttributes)
{
    auto mesh = Mesh
    {
        1,
        3,
        Vectors<float>{ 2, std::vector<float>{ 1.f, 1.f,  -1.f, 1.f,  0.f, -1.f } },
        Vectors<float>{},
        Vectors<float>{},
        Vectors<float>{ 3, std::vector<float>{ 1.f, 0.f, 0.f,  0.f, 1.f, 0.f,  0.f, 0.f, 1.f } },
        std::vector<Mesh::IndexType >{ 0, 1, 2 }
    };

    assertMeshBinding(mesh, mesh.positions, mesh.colors);
}