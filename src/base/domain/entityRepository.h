#pragma once

#include "base/dependencies.h"
#include "idFactory.h"

namespace dory::domain
{
    template<class TEntity>
    class VectorRepositoryTraverseIterator
    {
        private:
            typename std::vector<TEntity>::const_iterator current;
            typename std::vector<TEntity>::const_iterator end;

        public:
            VectorRepositoryTraverseIterator(typename std::vector<TEntity>::iterator begin, typename std::vector<TEntity>::iterator end):
                current(begin),
                end(end)
            {}

            const TEntity& next()
            {
                return *current++;
            }

            bool isEmpty() noexcept
            {
                return current == end;
            }

            void forEach(std::function<void(const TEntity&)> expression)
            {
                while(!isEmpty())
                {
                    const TEntity& entity = next();
                    expression(entity);
                }
            }
    };

    template<class TEntity>
    class IEntityRepository
    {
        public:
            virtual ~IEntityRepository() = default;

            virtual int getEntitiesCount() = 0;
            virtual VectorRepositoryTraverseIterator<TEntity> getTraverseIterator() = 0;
            virtual const TEntity& store(TEntity&& entity) = 0;
            virtual void remove(const TEntity& entity) = 0;
            virtual void remove(std::function<bool(const TEntity&)>) = 0;
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

            const TEntity& store(TEntity&& entity) override
            {
                return items.emplace_back(std::move(entity));
            }

            int getEntitiesCount() override
            {
                return items.size();
            }

            VectorRepositoryTraverseIterator<TEntity> getTraverseIterator() override
            {
                auto begin = items.begin();
                auto end = items.end();

                return VectorRepositoryTraverseIterator<TEntity>(begin, end);
            }

            void remove(const TEntity& entity) override
            {
                auto it = items.begin();
                auto end = items.end();

                for(; it != end; ++it)
                {
                    if(entity.id == (*it).id)
                    {
                        items.erase(it);
                        break;
                    }
                }
            }

            void remove(std::function<bool(const TEntity&)> expression) override
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

            VectorRepositoryTraverseIterator<TEntity> getTraverseIterator()
            {
                return repository->getTraverseIterator();
            }

            template<class TId>
            std::optional<TEntity> get(TId id)
            {
                auto iterator = repository->getTraverseIterator();

                while(!iterator.isEmpty())
                {
                    auto& entity = iterator.next();
                    if(entity.id == id)
                    {
                        return entity;
                    }
                }

                return {};
            }

            template<typename TField, typename F>
            std::optional<TEntity> get(TField searchValue, F expression)
            {
                auto iterator = repository->getTraverseIterator();

                while(!iterator.isEmpty())
                {
                    auto& entity = iterator.next();

                    if(expression(entity, searchValue))
                    {
                        return entity;
                    }
                }

                return {};
            }

            void forEach(std::function<void(const TEntity&)>&& expression)
            {
                repository->getTraverseIterator().forEach(std::move(expression));
            }

            template<typename TField, typename F>
            void list(TField searchValue, F expression, std::list<const TEntity&>& list)
            {
                auto iterator = repository->getTraverseIterator();

                while(!iterator.isEmpty())
                {
                    auto& entity = iterator.next();

                    if(expression(entity, searchValue))
                    {
                        list.emplace_back(entity);
                    }
                }
            }
    };
}