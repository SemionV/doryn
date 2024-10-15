#pragma once

#include <memory>
#include "services/iFileService.h"
#include "services/iLibraryService.h"



namespace dory::core
{
    namespace services
    {
        class ILibraryService;
    }

    struct Services
    {
        std::shared_ptr<services::IFileService> fileService;
        std::shared_ptr<services::ILibraryService> libraryService;
    };

    struct Registry
    {
        Services services;
    };
}