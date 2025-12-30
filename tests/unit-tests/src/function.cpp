#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <spdlog/fmt/fmt.h>
#include <dory/data-structures/function.h>
#include <dory/memory/allocators/segregationAllocator.h>
#include <allocatorBuilder.h>

TEST(FunctionWrapperTests, wrapDelegate)
{
    dory::test_utilities::AllocatorBuilder allocBuilder;
    const auto allocator = allocBuilder.build();
    dory::test_utilities::SegregationResource globalResource{ *allocator };

    int externalVariable = 1;

    auto delegate = [&externalVariable](const int param)
    {
        std::cout << "Delegate param: " << param << std::endl;
        std::cout << "Delegate external variable: " << externalVariable << std::endl;
    };

    const auto function = dory::data_structures::function::UniqueFunction<void(int)>{ &globalResource, delegate };
    function(2);

    std::cout << "UniqueFunction size: " << sizeof(decltype(function)) << std::endl;
}

class Handler
{
private:
    int _classVariable = 1;

public:
    void foo(const int param) const
    {
        std::cout << "Handler::foo param: " << param << std::endl;
        std::cout << "Handler::foo internal variable: " << _classVariable << std::endl;
    }
};

TEST(FunctionWrapperTests, wrapClassMember)
{
    dory::test_utilities::AllocatorBuilder allocBuilder;
    const auto allocator = allocBuilder.build();
    dory::test_utilities::SegregationResource globalResource{ *allocator };

    Handler h;
    const dory::data_structures::function::UniqueFunction<void(int)> function { &globalResource, dory::data_structures::function::bindMember(&h, &Handler::foo) };
    function(2);

    std::cout << "UniqueFunction size: " << sizeof(decltype(function)) << std::endl;
}

void freeFunction(const int param)
{
    std::cout << "freeFunction param: " << param << std::endl;
}

TEST(FunctionWrapperTests, wrapFreeFunction)
{
    dory::test_utilities::AllocatorBuilder allocBuilder;
    const auto allocator = allocBuilder.build();
    dory::test_utilities::SegregationResource globalResource{ *allocator };

    const auto function = dory::data_structures::function::UniqueFunction<void(int)>{ &globalResource, &freeFunction };
    function(2);

    std::cout << "UniqueFunction size: " << sizeof(decltype(function)) << std::endl;
}

TEST(FunctionWrapperTests, wrapLargeDelegate)
{
    dory::test_utilities::AllocatorBuilder allocBuilder;
    const auto allocator = allocBuilder.build();
    dory::test_utilities::SegregationResource globalResource{ *allocator };

    std::array<std::byte, 256> big{};

    auto bigLambda = [big](const int param) {
        std::cout << "LargeDelegate param: " << param << std::endl;
    };

    dory::data_structures::function::UniqueFunction<void(int)> function{ &globalResource, bigLambda }; // heap path
    function(1);

    std::cout << "UniqueFunction size: " << sizeof(decltype(function)) << std::endl;
}