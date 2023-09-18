#pragma once

#include "dependencies.h"
#include "idFactory.h"

namespace dory
{
    template<class TEntity>
    class ITraverseIterator
    {
        public:
            virtual std::optional<TEntity> next() = 0;
    };

    template<class TEntity>
    class CongruentRepositoryTraverseIterator: public ITraverseIterator<TEntity>
    {
        private:
            TEntity* currentEntity;
            TEntity* lastEntity;

        public:
            CongruentRepositoryTraverseIterator(TEntity* firstEntity, TEntity* lastEntity):
                currentEntity(firstEntity),
                lastEntity(lastEntity)
            {}

            std::optional<TEntity> next() override
            {
                if(currentEntity != lastEntity)
                {
                    return *currentEntity++;
                }

                return std::nullopt;
            }
    };

    template<class TEntity>
    class EmptyRepositoryTraverseIterator: public ITraverseIterator<TEntity>
    {
        public:
            std::optional<TEntity> next() override
            {
                return std::nullopt;
            }
    };

    template<class TEntity>
    class IEntityRepository
    {
        public:
            virtual int getEntitiesCount() = 0;
            virtual std::unique_ptr<ITraverseIterator<TEntity>> getTraverseIterator() = 0;
            virtual TEntity& store(TEntity&& entity) = 0;
            virtual void remove(TEntity& entity) = 0;
    };

    template<class TEntity, typename TId>
    class EntityRepository: public IEntityRepository<TEntity>
    {
        private:
            std::vector<TEntity> items;
            std::shared_ptr<IIdFactory<TId>> idFactory;

        public:
            EntityRepository(std::shared_ptr<IIdFactory<TId>> idFactory):
                idFactory(idFactory)
            {}

            TEntity& store(TEntity&& entity) override
            {
                entity.id = idFactory->generate();
                return items.emplace_back(std::forward<TEntity>(entity));
            }

            int getEntitiesCount() override
            {
                return items.size();
            }

            std::unique_ptr<ITraverseIterator<TEntity>> getTraverseIterator() override
            {
                auto begin = items.begin();
                auto end = items.end();

                if(items.size())
                {
                    return std::make_unique<CongruentRepositoryTraverseIterator<TEntity>>(&(*begin), &(*end));
                }

                return std::make_unique<EmptyRepositoryTraverseIterator<TEntity>>();
            }

            void remove(TEntity& entity) override
            {
                auto it = items.begin();
                auto end = items.end();

                for(; it != end; ++it)
                {
                    if((*it).id == entity.id)
                    {
                        items.erase(it);
                        break;
                    }
                }
            }
    };

    template<class TEntity>
    class RepositoryReader
    {
        private:
            std::shared_ptr<IEntityRepository<TEntity>> repository;

        public:
            RepositoryReader(std::shared_ptr<IEntityRepository<TEntity>> repository):
                repository(repository)
            {}

            int getEntitiesCount()
            {
                return repository->getEntitiesCount();
            }

            std::unique_ptr<ITraverseIterator<TEntity>> getTraverseIterator()
            {
                return repository->getTraverseIterator();
            }

            template<class TId>
            std::optional<TEntity> get(TId id)
            {
                auto iterator = repository->getTraverseIterator();
                auto entityMaybe = iterator->next();
                while(entityMaybe.has_value())
                {
                    auto entity = entityMaybe.value();
                    if(entity.id == id)
                    {
                        return entity;
                    }

                    entityMaybe = iterator->next();
                }

                return std::nullopt;
            }

            template<typename TField, typename F>
            std::optional<TEntity> get(TField searchValue, F expression)
            {
                auto iterator = repository->getTraverseIterator();
                auto entityMaybe = iterator->next();
                while(entityMaybe.has_value())
                {
                    auto entity = entityMaybe.value();
                    if(expression(entity, searchValue))
                    {
                        return entity;
                    }

                    entityMaybe = iterator->next();
                }

                return std::nullopt;
            }

            template<typename TField, typename F>
            void list(TField searchValue, F expression, std::list<std::reference_wrapper<TEntity>>& list)
            {
                auto iterator = repository->getTraverseIterator();
                auto entityMaybe = iterator->next();
                while(entityMaybe.has_value())
                {
                    auto entity = entityMaybe.value();
                    if(expression(entity, searchValue))
                    {
                        list.emplace_back(std::ref(entity));
                    }

                    entityMaybe = iterator->next();
                }
            }
    };
}