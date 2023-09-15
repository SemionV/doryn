#pragma once

#include "base/domain/entityRepository.h"
#include "base/domain/view.h"

namespace dory
{
    class ViewEntityRepository: public EntityRepository<ViewNi>
    {
        public:
            void getList(int windowId, std::list<ViewNi*>& list)
            {
                int count = getEntitiesCount();
                ViewNi* entity = getEntities();
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