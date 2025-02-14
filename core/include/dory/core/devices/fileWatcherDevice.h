#pragma once

#include <dory/core/devices/iFileWatcherDevice.h>
#include <efsw/efsw.hpp>

namespace dory::core
{
    class Registry;
}

namespace dory::core::devices
{
    class FileWatcherDevice: public IFileWatcherDevice, public efsw::FileWatchListener
    {
    private:
        Registry& _registry;
        std::unique_ptr<efsw::FileWatcher> _fileWatcher;

    public:
        explicit FileWatcherDevice(Registry& registry);

        void handleFileAction(efsw::WatchID watchid, const std::string& dir,
                               const std::string& filename, efsw::Action action,
                               std::string oldFilename) override;

        void updateWatches(resources::DataContext& context) override;
        void connect(resources::DataContext& context) override;
        void disconnect(resources::DataContext& context) override;
    };
}
