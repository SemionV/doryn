#pragma once

#include "base/domain/entityRepository.h"
#include "base/domain/entity.h"

namespace dory
{
    class ViewEntityRepository: public EntityRepository<View>
    {
        public:
            void getList(int windowId, std::list<View*>& list)
            {
                int count = getEntitiesCount();
                View* entity = getEntities();
                for(int i = 0; i < count; ++i)
                {
                    if(entity->windowId == windowId)
                    {
                        list.emplace_back(entity);
                    }

                    entity++;
                }
            }
    };
}