#pragma once

#include "base/dependencies.h"

namespace dory
{
    template<class TEntity>
    class EntityRepository
    {
        private:
            std::vector<TEntity> items;
            int idCounter;

        public:
            EntityRepository():
                idCounter(0)
            {}

            template<typename... Ts>
            TEntity& store(Ts&&... constructorArguments)
            {
                return items.emplace_back(TEntity(getNewItemId(), std::forward<Ts>(constructorArguments)...));
            }
            
            std::optional<TEntity> get(int id)
            {
                std::size_t size = items.size();
                for(std::size_t i = 0; i < size; ++i)
                {
                    auto item = items[i];
                    if(item.id == id)
                    {
                        return item;
                    }
                }

                return std::nullopt;
            }

            int getEntitiesCount()
            {
                return items.size();
            }

            TEntity* getEntities()
            {
                items.data();
            }

            void remove(int id)
            {
                auto it = items.begin();
                auto end = items.end();

                for(; it != end; ++it)
                {
                    if((*it).id == id)
                    {
                        items.erase(it);
                        break;
                    }
                }
            }

        protected:
            int getNewItemId()
            {
                return ++idCounter;
            }
    };
}