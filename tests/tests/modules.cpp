#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <dory/module.h>

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
    MOCK_METHOD(std::shared_ptr<dory::IDynamicModule>, loadModule, (const std::string moduleName));
};

TEST(RsourceHandleTests, lifetimeOfLibraryAndResource)
{
    auto library = std::make_shared<TestLibrary>();
    EXPECT_CALL(*library, isLoaded()).Times(2).WillOnce(Return(true)).WillOnce(Return(false));

    auto testController = std::make_shared<TestController>();
    EXPECT_CALL(*testController, update(1));
    EXPECT_CALL(*testController, callOperator());

    auto libraryHandle = dory::LibraryHandle{ library };

    auto libraryResource = dory::ResourceHandle<std::shared_ptr<TestController>>{libraryHandle, testController};

    {
        auto resource = libraryResource.lock();
        EXPECT_TRUE((bool)resource);

        if(resource)
        {
            (*resource)->update(1);
            (**resource)();
        }

        library.reset();

        auto resource2 = libraryResource.lock();
        EXPECT_FALSE((bool)resource2);
    }

    auto resource3 = libraryResource.lock();
    EXPECT_FALSE((bool)resource3);
}

class TestModule: dory::IDynamicModule
{
public:
    MOCK_METHOD(void, attach, (dory::LibraryHandle library), (final));
};

class InvokeSentinel
{
public:
    MOCK_METHOD(void, notifyCall, (int i));
};

class TestEventHandler
{
private:
    InvokeSentinel& _sentinel;

public:
    explicit TestEventHandler(InvokeSentinel& sentinel):
        _sentinel(sentinel)
    {}

    void handleEvent(int i)
    {
        _sentinel.notifyCall(i);
    }
};

class HandleResourceTests: public testing::Test
{
protected:
    using ResourceType = std::function<void(int)>;

    std::shared_ptr<TestLibrary> _library = std::make_shared<TestLibrary>();
    dory::LibraryHandle _libraryHandle = dory::LibraryHandle{ _library };
    InvokeSentinel _sentinel;

    void SetUp() override {
        EXPECT_CALL(_sentinel, notifyCall(1));
        EXPECT_CALL(_sentinel, notifyCall(4));
    }

    static void invokeHandler(const std::shared_ptr<dory::IResourceHandle<ResourceType>>& resourceHandle)
    {
        auto resource = resourceHandle->lock();
        if(resource)
        {
            (*resource).operator()(1);
            (*resource)(4);
        }
    }
};

TEST_F(HandleResourceTests, handleEventsByLambdaInLibrary)
{
    EXPECT_CALL(*_library, isLoaded()).WillOnce(Return(true));
    ResourceType handler = [this](int i) {
        _sentinel.notifyCall(i);
    };
    invokeHandler(dory::makeResourceHandle<ResourceType>(_libraryHandle, handler));
}

TEST_F(HandleResourceTests, handleEventsByBindInLibrary)
{
    EXPECT_CALL(*_library, isLoaded()).WillOnce(Return(true));
    auto object = TestEventHandler{ _sentinel };
    ResourceType handler2 = std::bind(&TestEventHandler::handleEvent, &object, std::placeholders::_1);
    invokeHandler(dory::makeResourceHandle<ResourceType>(_libraryHandle, handler2));
}

TEST_F(HandleResourceTests, handleEventsByLambdaInStaticLibrary)
{
    auto handler3 = [this](int i) {
        _sentinel.notifyCall(i);
    };

    invokeHandler(dory::makeResourceHandle<ResourceType>(std::move(handler3)));
}
TEST_F(HandleResourceTests, handleEventsByMemberFunctionLambdaInStaticLibrary)
{
    auto object2 = TestEventHandler{ _sentinel };
    ResourceType handler4 = [object = &object2, method = &TestEventHandler::handleEvent] <typename... Ts> (Ts&&... params) {
        std::invoke(method, object, std::forward<Ts>(params)...);
    };

    invokeHandler(dory::makeResourceHandle<ResourceType>(std::move(handler4)));
}