#include <dory/macros/assert.h>

namespace dory::assert
{
    AssertBase::AssertFailureHandlerType
    DebugAssert::assertFailureHandler = nullptr;

    AssertBase::AssertFailureHandlerType
    InHouseAssert::assertFailureHandler = nullptr;

    AssertBase::AssertFailureHandlerType
    Assert::assertFailureHandler = nullptr;
}