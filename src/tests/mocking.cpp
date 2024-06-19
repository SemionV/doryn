#include <gtest/gtest.h>
#include <gmock/gmock.h>

using ::testing::Return;
using ::testing::_;

template <typename T>
class MockMyTemplateClass {
public:
    MOCK_METHOD(void, DoSomething, (const T& value));
    MOCK_METHOD(T, GetValue, (), (const));
};

template <typename T>
class MyTemplateClassWrapper {
public:
    MyTemplateClassWrapper(MockMyTemplateClass<T>& mock) : mock_(mock) {}

    void DoSomething(const T& value) {
        mock_.DoSomething(value);
    }

    T GetValue() const {
        return mock_.GetValue();
    }

private:
    MockMyTemplateClass<T>& mock_;
};

// Test case
TEST(MyTemplateClassTest, DoSomethingTest) {
    // Create a mock object
    MockMyTemplateClass<int> mock;
    MyTemplateClassWrapper<int> wrapper(mock);

    // Set expectations
    EXPECT_CALL(mock, DoSomething(_)).Times(1);

    // Call the method
    wrapper.DoSomething(10);
}

TEST(MyTemplateClassTest, GetValueTest) {
    // Create a mock object
    MockMyTemplateClass<int> mock;
    MyTemplateClassWrapper<int> wrapper(mock);

    // Set expectations
    EXPECT_CALL(mock, GetValue()).WillOnce(Return(42));

    // Call the method and check the result
    int value = wrapper.GetValue();
    EXPECT_EQ(value, 42);
}

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

    EXPECT_CALL(meshRepositoryImpl, getMesh(1)).WillOnce(::testing::ReturnRef(mesh));

    auto resultMesh = meshRepository.getMesh(1);

    EXPECT_EQ(resultMesh.pointsCount, 5);
}