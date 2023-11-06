#pragma once

#include "base/dependencies.h"
#include "idFactory.h"

namespace dory::domain
{
    template<class TEntity>
    class RepositoryTraverseIterator
    {
        private:
            typename std::vector<TEntity>::const_iterator current;
            typename std::vector<TEntity>::const_iterator end;

        public:
            RepositoryTraverseIterator(typename std::vector<TEntity>::const_iterator begin, typename std::vector<TEntity>::const_iterator end):
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
            virtual RepositoryTraverseIterator<TEntity> getTraverseIterator() = 0;
            virtual const TEntity& store(TEntity&& entity) = 0;
            virtual void remove(const TEntity& entity) = 0;
            virtual void remove(const std::function<bool(const TEntity&)>&) = 0;
    };

    template<class TEntity,
        typename = std::enable_if_t<(!std::is_reference_v<TEntity>)>>
    class EntityRepository: public IEntityRepository<TEntity>
    {
        private:
            std::vector<TEntity> items;

        private:
            template<typename T, typename F> 
            inline bool matchEntry(const T& entry, const F& expression)
            {
                return expression(entry);
            }

            template<typename T> 
            inline bool matchEntry(const T& entry, const T& entity)
            {
                return entry.id == entity.id;
            }

            template<typename T>
            inline void removeEntry(const T& entityExpression)
            {
                auto it = items.cbegin();
                auto end = items.cend();

                for(; it != end; ++it)
                {
                    if(matchEntry(*it, entityExpression))
                    {
                        items.erase(it);
                        break;
                    }
                }
            }

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

            RepositoryTraverseIterator<TEntity> getTraverseIterator() override
            {
                auto begin = items.cbegin();
                auto end = items.cend();

                return RepositoryTraverseIterator<TEntity>(begin, end);
            }

            void remove(const TEntity& entity) override
            {
                removeEntry(entity);
            }

            void remove(const std::function<bool(const TEntity&)>& expression) override
            {
                removeEntry(expression);
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

            RepositoryTraverseIterator<TEntity> getTraverseIterator()
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