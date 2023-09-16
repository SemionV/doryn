#pragma once

#include "base/dependencies.h"

namespace dory
{
    
    template<class TEntity>
    class IEntityRepository
    {
        public:
            virtual int getEntitiesCount() = 0;
            virtual TEntity* getEntities() = 0;
            virtual TEntity& store(TEntity&& entity) = 0;
            virtual void remove(TEntity* entity) = 0;
    };

    template<class TEntity, typename TId>
    class EntityRepository: public IEntityRepository<TEntity>
    {
        private:
            std::vector<TEntity> items;
            TId idCounter;

        public:
            EntityRepository():
                idCounter(0)
            {}

            TEntity& store(TEntity&& entity) override
            {
                entity.id = getNewItemId();
                return items.emplace_back(std::forward<TEntity>(entity));
            }

            int getEntitiesCount() override
            {
                return items.size();
            }

            TEntity* getEntities() override
            {
                return items.data();
            }

            void remove(TEntity* entity) override
            {
                auto it = items.begin();
                auto end = items.end();

                for(; it != end; ++it)
                {
                    if((*it).id == entity->id)
                    {
                        items.erase(it);
                        break;
                    }
                }
            }

        protected:
            TId getNewItemId()
            {
                return ++idCounter;
            }
    };

    template<class TEntity>
    class EntityAccessor
    {
        private:
            std::shared_ptr<IEntityRepository<TEntity>> repository;

        public:
            EntityAccessor(std::shared_ptr<IEntityRepository<TEntity>> repository):
                repository(repository)
            {}

            int getEntitiesCount()
            {
                return repository->getEntitiesCount();
            }

            TEntity* getEntities()
            {
                return repository->getEntities();
            }

            template<class TId>
            TEntity* get(TId id)
            {
                int count = repository->getEntitiesCount();
                TEntity* entity = repository->getEntities();
                for(int i = 0; i < count; ++i)
                {
                    if(entity->id == id)
                    {
                        return entity;
                    }

                    entity++;
                }

                return nullptr;
            }

            template<typename TField, typename F>
            TEntity* get(TField searchValue, F expression)
            {
                int count = repository->getEntitiesCount();
                TEntity* entity = repository->getEntities();
                for(int i = 0; i < count; ++i)
                {
                    if(expression(entity, searchValue))
                    {
                        return entity;
                    }

                    entity++;
                }

                return nullptr;
            }

            template<typename TField, typename F>
            void list(TField searchValue, F expression, std::list<TEntity*>& list)
            {
                int count = repository->getEntitiesCount();
                TEntity* entity = repository->getEntities();
                for(int i = 0; i < count; ++i)
                {
                    if(expression(entity, searchValue))
                    {
                        list.emplace_back(entity);
                    }

                    entity++;
                }
            }
    };
}