#pragma once

#include <dory/core/repositories/iFileWatchRepository.h>
#include <dory/core/repository.h>

namespace dory::core::repositories
{
    class FileWatchRepository: public Repository<resources::entity::FileWatch, resources::IdType, IFileWatchRepository>
    {};
}