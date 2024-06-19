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

/*int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}*/