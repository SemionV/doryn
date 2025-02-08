#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <dory/core/registry.h>
#include <dory/core/services/pipelineService.h>
#include <dory/core/repositories/iPipelineRepository.h>
#include <dory/core/iController.h>

#include <utility>

#include "mocks/logService.h"

using namespace dory;
using namespace core;
using namespace services;
using namespace resources;
using namespace entities;
using namespace configuration;
using namespace repositories;
using namespace testing;

class PipelineRepositoryMock final : public IPipelineRepository
{
public:
    MOCK_METHOD(std::span<EntityType>, getPipelineNodes, ());
    MOCK_METHOD(IdType, addNode, (const EntityType& pipelineNode));
    MOCK_METHOD(IdType, addTriggerNode, (IdType parentNode, const generic::extension::LibraryHandle& libraryHandle, const EntityType::UpdateTriggerType& updateTrigger));
    MOCK_METHOD(IdType, addNode, (IdType parentNode, const generic::extension::LibraryHandle& libraryHandle, EntityType::ControllerPointerType controller));
    MOCK_METHOD(IdType, addNode, (IdType parentNode, const generic::extension::LibraryHandle& libraryHandle, EntityType::ControllerPointerType controller, const EntityType::UpdateTriggerType& updateTrigger));
    MOCK_METHOD(IdType, addNode, (IdType parentNode, const generic::extension::LibraryHandle& libraryHandle, const EntityType::UpdateFunctionType& updateFunction));
    MOCK_METHOD(IdType, addNode, (IdType parentNode, const generic::extension::LibraryHandle& libraryHandle, const EntityType::UpdateFunctionType& updateFunction, const EntityType::UpdateTriggerType& updateTrigger));
    MOCK_METHOD(void, removeNode, (IdType id));
    MOCK_METHOD(EntityType*, getNode, (const Name& name));
};

class ControllerMock final: public IController
{
public:
    MOCK_METHOD(bool, initialize, (resources::IdType nodeId, resources::DataContext& context));
    MOCK_METHOD(void, stop, (resources::IdType nodeId, resources::DataContext& context));
    MOCK_METHOD(void, update, (resources::IdType nodeId, const generic::model::TimeSpan& timeStep, resources::DataContext& context));
};

class TriggerMock
{
public:
    MOCK_METHOD(NodeUpdateCounter, trigger, (resources::IdType nodeId, const generic::model::TimeSpan& timeStep, resources::DataContext& context));
};

class PipelineNodeFactory;

class FluentNodeFactoryInterface
{
private:
    const PipelineNodeFactory& _factory;
    PipelineNode _node;

public:
    explicit FluentNodeFactoryInterface(const PipelineNodeFactory& factory, const PipelineNode& node):
        _factory(factory),
        _node(node)
    {}

    FluentNodeFactoryInterface& attachTrigger(const generic::model::TimeSpan& deltaTime = {}, const std::size_t triggerCount = 1, const NodeUpdateCounter& updateTrigger = {});
    FluentNodeFactoryInterface& attachController(const generic::model::TimeSpan& deltaTime = {}, const std::size_t updateCount = 1);
    FluentNodeFactoryInterface& attachUpdateFunction(const generic::model::TimeSpan& deltaTime = {}, const std::size_t updateCount = 1);
    FluentNodeFactoryInterface& getId(IdType& id);
    PipelineNode get();
};


class PipelineNodeFactory
{
private:
    const generic::extension::LibraryHandle& _libraryHandle;
    DataContext& _dataContex;
    const Matcher<DataContext&> _contextMatcher;
    std::size_t _idCounter { 0 };

public:
    explicit PipelineNodeFactory(DataContext& dataContex,
        const generic::extension::LibraryHandle& libraryHandle) :
        _libraryHandle(libraryHandle),
        _dataContex(dataContex),
        _contextMatcher(Truly([this](auto& ctx) { return &ctx == &_dataContex; }))
    {}

    FluentNodeFactoryInterface createNode(const IdType parentId = nullId, const std::string& name = "")
    {
        PipelineNode node {};
        node.id = ++_idCounter;
        node.parentNodeId = parentId;
        node.name = name;

        return FluentNodeFactoryInterface{ *this, node };
    }

    template<typename TTrigger = TriggerMock>
    void attachTrigger(PipelineNode& node, const generic::model::TimeSpan& deltaTime, const std::size_t triggerCount , const NodeUpdateCounter& updateTrigger) const
    {
        auto trigger = std::make_shared<TTrigger>();
        EXPECT_CALL(*trigger, trigger(Eq(node.id), Eq(deltaTime), _contextMatcher))
            .Times(triggerCount)
            .WillRepeatedly(Return( NodeUpdateCounter{ updateTrigger.count, updateTrigger.deltaTime }));

        std::function triggerLambda = [trigger](const IdType nodeId, const generic::model::TimeSpan& timeStep, DataContext& context) {
            return trigger->trigger(nodeId, timeStep, context);
        };
        const auto triggerHandle = generic::extension::ResourceHandle{ _libraryHandle, std::move(triggerLambda) };
        node.triggerFunction = triggerHandle;
    }

    template<typename TController = ControllerMock>
    void attachController(PipelineNode& node, const generic::model::TimeSpan& deltaTime, const std::size_t updateCount) const
    {
        const auto controller = std::make_shared<TController>();
        EXPECT_CALL(*controller, update(Eq(node.id), Eq(deltaTime), _contextMatcher)).Times(updateCount);

        auto handle = generic::extension::ResourceHandle{ _libraryHandle, std::static_pointer_cast<IController>(std::move(controller)) };
        node.attachedController = handle;
    }

    template<typename TController = ControllerMock>
    void attachUpdateFunction(PipelineNode& node, const generic::model::TimeSpan& deltaTime , const std::size_t updateCount) const
    {
        const auto controller = std::make_shared<TController>();
        EXPECT_CALL(*controller, update(Eq(node.id), Eq(deltaTime), _contextMatcher)).Times(updateCount);
        std::function updateFunction = [controller](const IdType nodeId, const generic::model::TimeSpan& timeStep, DataContext& context) {
            controller->update(nodeId, timeStep, context);
        };

        const auto handle = generic::extension::ResourceHandle{ _libraryHandle, std::move(updateFunction) };
        node.updateFunction = handle;
    }
};

FluentNodeFactoryInterface& FluentNodeFactoryInterface::attachTrigger(const generic::model::TimeSpan& deltaTime, const std::size_t triggerCount, const NodeUpdateCounter& updateTrigger)
{
    _factory.attachTrigger(_node, deltaTime, triggerCount, updateTrigger);
    return *this;
}

FluentNodeFactoryInterface& FluentNodeFactoryInterface::attachController(const generic::model::TimeSpan& deltaTime , const std::size_t updateCount)
{
    _factory.attachController(_node, deltaTime, updateCount);
    return *this;
}

FluentNodeFactoryInterface& FluentNodeFactoryInterface::attachUpdateFunction(const generic::model::TimeSpan& deltaTime, const std::size_t updateCount)
{
    _factory.attachUpdateFunction(_node, deltaTime, updateCount);
    return *this;
}

FluentNodeFactoryInterface& FluentNodeFactoryInterface::getId(IdType& id)
{
    id = _node.id;
    return *this;
}

PipelineNode FluentNodeFactoryInterface::get()
{
    return _node;
}

void setupRegistry(Registry& registry, const generic::extension::LibraryHandle& libraryHandle, std::vector<PipelineNode>& nodes)
{
    const auto pipelineRepo = std::make_shared<PipelineRepositoryMock>();
    registry.set<IPipelineRepository>(libraryHandle, pipelineRepo);
    EXPECT_CALL(*pipelineRepo, getPipelineNodes()).WillOnce(Return(std::span { nodes }));

    const auto logService = std::make_shared<LogServiceMock>();
    registry.set<ILogService>(libraryHandle, logService);
    EXPECT_CALL(*logService, error(_)).Times(0);
}

TEST(PipelineService, update)
{
    Registry registry{};
    Configuration configuration {};
    Localization localization {};
    DataContext context { configuration, localization};
    const generic::extension::LibraryHandle libraryHandle {};
    constexpr auto deltaTime = std::chrono::nanoseconds { 1 };

    PipelineNodeFactory factory {context, libraryHandle};

    std::vector nodes {
        factory.createNode().get(),
        factory.createNode().attachController(deltaTime).get(),
        factory.createNode().attachUpdateFunction(deltaTime).get(),
    };

    setupRegistry(registry, libraryHandle, nodes);

    auto pipelineService = PipelineService{ registry };
    pipelineService.update(context, deltaTime);
}

TEST(PipelineService, multipleUpdate)
{
    Registry registry{};
    Configuration configuration {};
    Localization localization {};
    DataContext context { configuration, localization};
    const generic::extension::LibraryHandle libraryHandle {};
    constexpr auto deltaTime = std::chrono::nanoseconds { 1 };
    constexpr auto deltaTime2 = std::chrono::nanoseconds { 3 };
    IdType parentId {};
    IdType parentId2 {};
    IdType parentId3 {};

    PipelineNodeFactory factory {context, libraryHandle};

    std::vector nodes {
        factory.createNode().attachController(deltaTime).get(),
        factory.createNode().attachTrigger(deltaTime, 1, { 2, deltaTime2 }).getId(parentId).get(),
            factory.createNode(parentId).attachController(deltaTime2, 2).get(),
            factory.createNode(parentId).attachController(deltaTime2, 2).get(),
            factory.createNode(parentId).attachTrigger(deltaTime2, 2, { 4, deltaTime }).getId(parentId3).get(),
                factory.createNode(parentId3).attachController(deltaTime, 2 * 4).get(),
            factory.createNode(parentId).attachController(deltaTime2, 2).get(),
        factory.createNode().attachTrigger(deltaTime, 1, { 3, deltaTime2 }).getId(parentId2).get(),
            factory.createNode(parentId2).attachController(deltaTime2, 3).get()
    };

    setupRegistry(registry, libraryHandle, nodes);

    auto pipelineService = PipelineService{ registry };
    pipelineService.update(context, deltaTime);
}

TEST(PipelineService, declineUpdate)
{
    Registry registry{};
    Configuration configuration {};
    Localization localization {};
    DataContext context { configuration, localization};
    const generic::extension::LibraryHandle libraryHandle {};
    constexpr auto deltaTime = std::chrono::nanoseconds { 1 };
    IdType parentId {};

    PipelineNodeFactory factory {context, libraryHandle};

    std::vector nodes {
        factory.createNode().attachController(deltaTime).get(),
        factory.createNode().attachTrigger(deltaTime, 1, { 0, deltaTime }).attachController(deltaTime, 0).getId(parentId).get(),
            factory.createNode(parentId).attachController(deltaTime, 0).get(),
            factory.createNode(parentId).attachController(deltaTime, 0).get(),
        factory.createNode().attachController(deltaTime).get()
    };

    setupRegistry(registry, libraryHandle, nodes);

    auto pipelineService = PipelineService{ registry };
    pipelineService.update(context, deltaTime);
}