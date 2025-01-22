#pragma once

#include <dory/core/services/iLogService.h>
#include <dory/core/implementation.h>

template<typename TMessage>
class LogServiceMockTemplate: dory::core::services::ILogService
{
public:
    MOCK_METHOD(void, trace, (const TMessage& message));
    MOCK_METHOD(void, debug, (const TMessage& message));
    MOCK_METHOD(void, information, (const TMessage& message));
    MOCK_METHOD(void, warning, (const TMessage& message));
    MOCK_METHOD(void, error, (const TMessage& message));
    MOCK_METHOD(void, critical, (const TMessage& message));
};

template<typename TMessage, typename TPolicy, typename TState>
class LogServiceGenericMock: public dory::core::implementation::ImplementationLevel<TPolicy, TState>
{
public:
    MOCK_METHOD(void, trace, (const TMessage& message));
    MOCK_METHOD(void, debug, (const TMessage& message));
    MOCK_METHOD(void, information, (const TMessage& message));
    MOCK_METHOD(void, warning, (const TMessage& message));
    MOCK_METHOD(void, error, (const TMessage& message));
    MOCK_METHOD(void, critical, (const TMessage& message));
};

struct LogServiceMockPolicy: dory::core::implementation::ImplementationPolicy<dory::core::implementation::ImplementationList<LogServiceGenericMock>>
{};

using LogServiceMock = dory::core::implementation::Implementation<dory::generic::TypeList<dory::core::services::ILogService>, dory::core::services::ILogService::MessageTypes, LogServiceMockPolicy>;