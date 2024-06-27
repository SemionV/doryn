#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <dory/base.h>

using testing::Return;
using testing::ReturnRef;

struct Mesh
{
    int pointsCount = 0;
};

template<typename TImplementation>
class IMeshRepository
{
private:
    TImplementation& _implementation;

public:
    virtual ~IMeshRepository() = default;

    explicit IMeshRepository(TImplementation& implementation):
            _implementation(implementation)
    {}

    Mesh& getMesh(int id)
    {
        return _implementation.getMesh(id);
    }
};

class MeshRepository
{
private:
    Mesh _mesh = Mesh{ 5 };

public:
    Mesh& getMesh(int id)
    {
        return _mesh;
    }
};

class MeshRepositoryMock
{
public:
    MOCK_METHOD(Mesh&, getMesh, (int id));
};

struct Registry
{
private:
    MeshRepository meshRepositoryImpl;
public:
    IMeshRepository<MeshRepository> meshRepository = IMeshRepository<MeshRepository>{meshRepositoryImpl};
};

TEST(MeshRepositoryTests, getMeshTest)
{
    MeshRepositoryMock meshRepositoryImpl;
    auto meshRepository = IMeshRepository<MeshRepositoryMock>{meshRepositoryImpl};

    auto mesh = Mesh{5};

    EXPECT_CALL(meshRepositoryImpl, getMesh(1)).WillOnce(ReturnRef(mesh));

    auto resultMesh = meshRepository.getMesh(1);

    EXPECT_EQ(resultMesh.pointsCount, 5);
}

class TestController
{
public:
    MOCK_METHOD(void, update, (int step));
    MOCK_METHOD(void, callOperator, ());

    void operator()()
    {
        callOperator();
    }
};

struct TestModuleContext
{};

class TestLibrary: public dory::ILibrary
{
public:
    MOCK_METHOD(bool, isLoaded, (), (final));
    MOCK_METHOD(void, load, (const std::filesystem::path& libraryPath));
    MOCK_METHOD(std::shared_ptr<dory::IDynamicModule<TestModuleContext>>, loadModule, (const std::string moduleName));
};

TEST(RsourceHandleTests, lifetimeOfLibraryAndResource)
{
    auto library = std::make_shared<TestLibrary>();
    EXPECT_CALL(*library, isLoaded()).Times(2).WillOnce(Return(true)).WillOnce(Return(false));

    auto testController = std::make_unique<TestController>();
    EXPECT_CALL(*testController, update(1));
    EXPECT_CALL(*testController, callOperator());

    auto libraryHandle = dory::LibraryHandle{ library };

    auto libraryResource = dory::ResourceHandle<TestController>{libraryHandle, testController.get()};

    {
        auto resource = libraryResource.lock();
        EXPECT_TRUE((bool)resource);

        if(resource)
        {
            resource->update(1);
            (*resource)();
        }

        library.reset();

        auto resource2 = libraryResource.lock();
        EXPECT_FALSE((bool)resource2);
    }

    auto resource3 = libraryResource.lock();
    EXPECT_FALSE((bool)resource3);
}

class TestModule: dory::IDynamicModule<TestModuleContext>
{
public:
    MOCK_METHOD(void, attach, (dory::LibraryHandle library, TestModuleContext& moduleContext), (final));
};

class TestEventHandler
{
public:
    void handleEvent(int i)
    {
        std::cout << "handler 2: " << i << "\n";
    }
};

class HandleResourceTests: public testing::Test
{
protected:
    using ResourceType = std::function<void(int)>;

    std::shared_ptr<TestLibrary> _library = std::make_shared<TestLibrary>();
    dory::LibraryHandle _libraryHandle = dory::LibraryHandle{ _library };

    void SetUp() override {
    }

    void invokeHandler(std::shared_ptr<dory::IResourceHandle<ResourceType>> resourceHandle)
    {
        auto resource = resourceHandle->lock();
        if(resource)
        {
            resource->operator()(1);
            (*resource)(4);
        }
    }
};

TEST_F(HandleResourceTests, handleEventsByLambdaInLibrary)
{
    EXPECT_CALL(*_library, isLoaded()).WillOnce(Return(true));
    ResourceType handler = [](int i){std::cout << "handler: " << i << "\n";};

    invokeHandler(std::make_shared<dory::ResourceHandle<ResourceType>>(_libraryHandle, &handler));
}

TEST_F(HandleResourceTests, handleEventsByBindInLibrary)
{
    EXPECT_CALL(*_library, isLoaded()).WillOnce(Return(true));
    TestEventHandler object;
    ResourceType handler2 = std::bind(&TestEventHandler::handleEvent, &object, std::placeholders::_1);

    invokeHandler(std::make_shared<dory::ResourceHandle<ResourceType>>(_libraryHandle, &handler2));
}

TEST_F(HandleResourceTests, handleEventsByLambdaInStaticLibrary)
{
    auto handler3 = [](int i){std::cout << "handler 3: " << i << "\n";};

    invokeHandler(std::make_shared<dory::StaticResourceHandle<ResourceType>>(std::move(handler3)));
}
TEST_F(HandleResourceTests, handleEventsByMemberFunctionLambdaInStaticLibrary)
{
    TestEventHandler object2;
    ResourceType handler4 = [object = &object2, method = &TestEventHandler::handleEvent] <typename... Ts> (Ts&&... params) {
        std::invoke(method, object, std::forward<Ts>(params)...);
    };

    invokeHandler(std::make_shared<dory::StaticResourceHandle<ResourceType>>(std::move(handler4)));
}