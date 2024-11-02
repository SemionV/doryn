#include <dory/core/registry.h>
#include <dory/core/devices/fileWatcherDevice.h>
#include <dory/core/repositories/iFileWatchRepository.h>
#include <dory/core/resources/fileWatch.h>
#include <iostream>

namespace dory::core::devices
{
    FileWatcherDevice::FileWatcherDevice(Registry &registry):
            _registry(registry),
            _updateListener(std::make_unique<FileWatcherDevice::UpdateListener>()),
            _fileWatcher(std::make_unique<efsw::FileWatcher>(true))
    {}

    void FileWatcherDevice::UpdateListener::handleFileAction(efsw::WatchID watchid, const std::string &dir,
                                                             const std::string &filename, efsw::Action action,
                                                             std::string oldFilename)
    {
        switch ( action ) {
            case efsw::Actions::Add:
                std::cout << "DIR (" << dir << ") FILE (" << filename << ") has event Added"
                          << std::endl;
                break;
            case efsw::Actions::Delete:
                std::cout << "DIR (" << dir << ") FILE (" << filename << ") has event Delete"
                          << std::endl;
                break;
            case efsw::Actions::Modified:
                std::cout << "DIR (" << dir << ") FILE (" << filename << ") has event Modified"
                          << std::endl;
                break;
            case efsw::Actions::Moved:
                std::cout << "DIR (" << dir << ") FILE (" << filename << ") has event Moved from ("
                          << oldFilename << ")" << std::endl;
                break;
            default:
                std::cout << "Should never happen!" << std::endl;
        }
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
                    auto id = _fileWatcher->addWatch(fileWatch.directory, _updateListener.get(), false);
                    auto specificData = std::make_shared<resources::entity::EntropiaFileWatch>();
                    if(fileWatch.specificData)
                    {
                        specificData->watchId = id;
                        fileWatch.specificData = specificData;
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