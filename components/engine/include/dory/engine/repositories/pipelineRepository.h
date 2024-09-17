#pragma once

#include "dory/engine/resources/object.h"
#include "entityRepository.h"
#include "dory/engine/resources/entity.h"
#include "dory/generics/typeTraits.h"

namespace dory::engine::repositories
{
    template<typename T>
    bool compareNodes(const std::shared_ptr<T>& a, const std::shared_ptr<T>& b)
    {
        return a->nodeEntity.priority < b->nodeEntity.priority;
    }

    template<typename TImplementation, typename TDataContext>
    class IPipelineRepository: public IEntityRepository<TImplementation, resources::entity::PipelineNode<TDataContext>, resources::entity::IdType>
    {
    public:
        using IdType = resources::entity::IdType;

        std::list<std::shared_ptr<resources::object::PipelineNode<TDataContext>>> getPipeline()
        {
            return this->toImplementation()->getPipelineImpl();
        }
    };

    template<typename TDataContext, typename TEntity, typename TIdType = resources::entity::IdType>
    class PipelineRepository:
            public IPipelineRepository<PipelineRepository<TDataContext, TEntity, TIdType>, TDataContext>,
            public EntityRepository<TEntity, TIdType>
    {
    private:
        using InterfaceType = IPipelineRepository<PipelineRepository<TDataContext, TEntity, TIdType>, TDataContext>;

    public:
        PipelineRepository() = default;

        PipelineRepository(std::initializer_list<TEntity>&& entities):
                EntityRepository<TEntity, TIdType>(std::move(entities))
        {}

        std::list<std::shared_ptr<resources::object::PipelineNode<TDataContext>>> getPipelineImpl()
        {
            std::list<std::shared_ptr<resources::object::PipelineNode<TDataContext>>> nodes;

            InterfaceType::forEach([this, &nodes](const TEntity& nodeEntity)
                                   {
                                       if(nodeEntity.parentNodeId == resources::entity::nullId)
                                       {
                                           nodes.emplace_back(loadNode(nodeEntity));
                                       }
                                   });

            nodes.sort(compareNodes<resources::object::PipelineNode<TDataContext>>);

            return nodes;
        }

    private:
        std::shared_ptr<resources::object::PipelineNode<TDataContext>> loadNode(const TEntity& nodeEntity)
        {
            auto node = std::make_shared<resources::object::PipelineNode<TDataContext>>(nodeEntity);

            InterfaceType::forEach([this, &node](const auto& nodeEntity)
                                   {
                                       if(nodeEntity.parentNodeId == node->nodeEntity.id)
                                       {
                                           node->children.emplace_back(loadNode(nodeEntity));
                                       }
                                   });

            node->children.sort(compareNodes<resources::object::PipelineNode<TDataContext>>);

            return node;
        }
    };
}