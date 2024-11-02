#include <dory/core/registry.h>
#include <dory/core/devices/fileWatcherDevice.h>
#include <dory/core/repositories/iFileWatchRepository.h>
#include <dory/core/resources/fileWatch.h>
#include <iostream>

namespace dory::core::devices
{
    FileWatcherDevice::FileWatcherDevice(Registry &registry):
            _registry(registry),
            _fileWatcher(std::make_unique<efsw::FileWatcher>(true))
    {}

    void FileWatcherDevice::handleFileAction(efsw::WatchID watchid, const std::string& dir,
                                                             const std::string& filename, efsw::Action action,
                                                             std::string oldFilename)
    {
        _registry.get<repositories::IFileWatchRepository>([this, &watchid, &filename](repositories::IFileWatchRepository* fileWatchRepository){
            auto fileWatches = fileWatchRepository->getAll();
            for(auto& fileWatch : fileWatches)
            {
                if(fileWatch.fileWatchSystem == resources::FileWatchSystem::entropia && fileWatch.specificData)
                {
                    auto specificData = std::static_pointer_cast<resources::entity::EntropiaFileWatch>(fileWatch.specificData);
                    if(specificData->watchId == watchid && fileWatch.file == filename && efsw::Actions::Modified)
                    {
                        auto logger = _registry.get<services::ILogService, resources::Logger::App>();
                        if(logger)
                        {
                            logger->information("File is modified: " + fileWatch.file);
                        }
                    }
                }
            }
        });
    }

    void FileWatcherDevice::updateWatches()
    {
        _registry.get<repositories::IFileWatchRepository>([this](repositories::IFileWatchRepository* fileWatchRepository) {
            auto fileWatches = fileWatchRepository->getAll();

            _fileWatcher = std::make_unique<efsw::FileWatcher>(true);

            for(auto& fileWatch : fileWatches)
            {
                if(fileWatch.fileWatchSystem == resources::FileWatchSystem::entropia)
                {
                    auto id = _fileWatcher->addWatch(fileWatch.directory, this, false);
                    auto specificData = std::make_shared<resources::entity::EntropiaFileWatch>();
                    if(specificData)
                    {
                        specificData->watchId = id;
                        fileWatch.specificData = specificData;
                        fileWatchRepository->store(fileWatch);
                    }
                }
            }

            _fileWatcher->watch();
        });
    }

    void FileWatcherDevice::connect(resources::DataContext& context)
    {
        updateWatches();
    }

    void FileWatcherDevice::disconnect(resources::DataContext& context)
    {}
}