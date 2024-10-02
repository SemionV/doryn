#pragma once

#include <memory>
#include "services/iFileService.h"

namespace dory::core
{
    struct Services
    {
        std::shared_ptr<services::IFileService> fileService;
    };

    struct Registry
    {
        Services services;
    };
}