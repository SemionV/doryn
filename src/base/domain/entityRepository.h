#pragma once

#include "base/dependencies.h"
#include "idFactory.h"

namespace dory::domain
{
    template<class TEntity>
    class ITraverseIterator
    {
        public:
            virtual ~ITraverseIterator() = default;

            virtual TEntity* next() = 0;
            virtual void forEach(std::function<void(TEntity&)>) = 0;
    };

    template<class TEntity>
    class VectorRepositoryTraverseIterator: public ITraverseIterator<TEntity>
    {
        private:
            typename std::vector<TEntity>::iterator current;
            typename std::vector<TEntity>::iterator end;

        public:
            VectorRepositoryTraverseIterator(typename std::vector<TEntity>::iterator begin, typename std::vector<TEntity>::iterator end):
                current(begin),
                end(end)
            {}

            TEntity* next() override
            {
                if(current != end)
                {
                    return &(*current++);
                }

                return nullptr;
            }

            void forEach(std::function<void(TEntity&)> expression) override
            {
                TEntity* entity = next();
                while(entity)
                {
                    expression(*entity);
                    entity = next();
                }
            }
    };

    template<class TEntity>
    class EmptyRepositoryTraverseIterator: public ITraverseIterator<TEntity>
    {
        public:
            TEntity* next() override
            {
                return nullptr;
            }

            void forEach(std::function<void(TEntity&)>) override
            {
            }
    };

    template<class TEntity>
    class IEntityRepository
    {
        public:
            virtual ~IEntityRepository() = default;

            virtual int getEntitiesCount() = 0;
            virtual std::unique_ptr<ITraverseIterator<TEntity>> getTraverseIterator() = 0;
            virtual TEntity& store(TEntity&& entity) = 0;
            virtual void remove(TEntity* entity) = 0;
            virtual void remove(std::function<bool(TEntity&)>) = 0;
    };

    template<class TEntity,
        typename = std::enable_if_t<(!std::is_reference_v<TEntity>)>>
    class EntityRepository: public IEntityRepository<TEntity>
    {
        private:
            std::vector<TEntity> items;

        public:
            EntityRepository()
            {
            }

            EntityRepository(std::initializer_list<TEntity>&& data):
                items(std::forward<std::vector<TEntity>>(data))
            {
            }

            TEntity& store(TEntity&& entity) override
            {
                return items.emplace_back(std::move(entity));
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
                    return std::make_unique<VectorRepositoryTraverseIterator<TEntity>>(begin, end);
                }

                return std::make_unique<EmptyRepositoryTraverseIterator<TEntity>>();
            }

            void remove(TEntity* entity) override
            {
                auto it = items.begin();
                auto end = items.end();

                for(; it != end; ++it)
                {
                    if(&(*it) == entity)
                    {
                        items.erase(it);
                        break;
                    }
                }
            }

            void remove(std::function<bool(TEntity&)> expression) override
            {
                auto it = items.begin();
                auto end = items.end();

                for(; it != end; ++it)
                {
                    if(expression((*it)))
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
            TEntity* get(TId id)
            {
                auto iterator = repository->getTraverseIterator();
                auto entity = iterator->next();
                while(entity)
                {
                    if(entity->id == id)
                    {
                        return entity;
                    }

                    entity = iterator->next();
                }

                return nullptr;
            }

            template<typename TField, typename F>
            TEntity* get(TField searchValue, F expression)
            {
                auto iterator = repository->getTraverseIterator();
                auto entity = iterator->next();
                while(entity)
                {
                    if(expression(entity, searchValue))
                    {
                        return entity;
                    }

                    entity = iterator->next();
                }

                return nullptr;
            }

            template<typename TField, typename F>
            void list(TField searchValue, F expression, std::list<TEntity*>& list)
            {
                auto iterator = repository->getTraverseIterator();
                auto entity = iterator->next();
                while(entity)
                {
                    if(expression(entity, searchValue))
                    {
                        list.emplace_back(entity);
                    }

                    entity = iterator->next();
                }
            }
    };
}