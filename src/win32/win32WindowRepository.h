#pragma once

#include "dependencies.h"
#include "win32Window.h"

namespace dory::win32
{
    class Win32WindowRespository: public EntityRepository<Win32Window>
    {
        public:
            Win32Window* getByHWND(HWND hWnd)
            {
                int count = getEntitiesCount();
                Win32Window* entity = getEntities();
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