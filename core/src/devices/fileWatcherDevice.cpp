#include <dory/core/registry.h>
#include <dory/core/devices/fileWatcherDevice.h>

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
        switch (action)
        {
            case efsw::Actions::Modified:
            {
                auto dirPath = std::filesystem::path{dir};
                auto event = events::filesystem::FileModified{ { dirPath / filename } };

                auto dispatcher = _registry.get<events::filesystem::Bundle::IDispatcher>();
                if(dispatcher)
                {
                    dispatcher->charge(event);
                }

                break;
            }
            case efsw::Actions::Add:
            case efsw::Actions::Delete:
            case efsw::Actions::Moved:
                break;
        }

        /*std::error_code ecode;
        auto path3 = std::filesystem::path{dir}.append(filename);
        bool equal2 = std::filesystem::equivalent(path3, "modules/renderer-opengl.so", ecode);*/

        /*_registry.get<repositories::IFileWatchRepository>([this, &watchid, &filename](repositories::IFileWatchRepository* fileWatchRepository){
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
        });*/
    }

    void FileWatcherDevice::updateWatches(resources::DataContext& context)
    {
        _fileWatcher = std::make_unique<efsw::FileWatcher>(true);

        for(const auto& fsWatch : context.configuration.fileSystemWatches)
        {
            _fileWatcher->addWatch(fsWatch.directory, this, fsWatch.recursive);
        }

        _fileWatcher->watch();
    }

    void FileWatcherDevice::connect(resources::DataContext& context)
    {
        updateWatches(context);
    }

    void FileWatcherDevice::disconnect(resources::DataContext& context)
    {}
}