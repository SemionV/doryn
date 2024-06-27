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

        if(*resource)
        {
            (*resource)->update(1);
            (**resource)();
        }

        library.reset();

        auto resource2 = libraryResource.lock();
        EXPECT_FALSE((bool)(*resource2));
    }

    auto resource3 = libraryResource.lock();
    EXPECT_FALSE((bool)(*resource3));
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

TEST(LibraryTests, handleEventsInLibrary)
{
    auto library = std::make_shared<TestLibrary>();
    auto libraryHandle = dory::LibraryHandle{ library };
    EXPECT_CALL(*library, isLoaded()).WillRepeatedly(Return(true));

    using ResourceType = std::function<void(int)>;

    ResourceType handler = [](int i){std::cout << "handler: " << i << "\n";};

    std::unordered_map<int, std::shared_ptr<dory::IResourceHandle<ResourceType>>> handlers;
    handlers[1] = std::make_shared<dory::ResourceHandle<ResourceType>>(libraryHandle, &handler);
    auto resourceHandle = handlers[1];

    {
        auto resource = resourceHandle->lock();
        if(*resource)
        {
            (*resource)->operator()(1);
            (**resource)(4);
        }
    }

    TestEventHandler object;
    //use template lambda
    //ResourceType handler2 = [object = &object](auto&& param) { object->handleEvent(std::forward<decltype(param)>(param)); };
    ResourceType handler2 = std::bind(&TestEventHandler::handleEvent, &object, std::placeholders::_1);
    handlers[2] = std::make_shared<dory::ResourceHandle<ResourceType>>(libraryHandle, &handler2);
    auto resourceHandle2 = handlers[2];

    {
        auto resource = resourceHandle2->lock();
        if(*resource)
        {
            (*resource)->operator()(1);
            (**resource)(4);
        }
    }

    auto handler3 = [](int i){std::cout << "handler 3: " << i << "\n";};
    handlers[3] = std::make_shared<dory::StaticResourceHandle<ResourceType>>(std::move(handler3));
    auto resourceHandle3 = handlers[3];

    {
        auto resource = resourceHandle3->lock();
        if(*resource)
        {
            (*resource)->operator()(1);
            (**resource)(4);
        }
    }
}