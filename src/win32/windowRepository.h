#pragma once

#include "dependencies.h"
#include "window.h"

namespace dory::win32
{
    class WindowRespository: public EntityRepository<Window>
    {
        public:
            Window* getByHWND(HWND hWnd)
            {
                int count = getEntitiesCount();
                Window* entity = getEntities();
                for(int i = 0; i < count; ++i)
                {
                    if(entity->hWnd == hWnd)
                    {
                        return entity;
                    }

                    entity++;
                }

                return nullptr;
            }
    };
}