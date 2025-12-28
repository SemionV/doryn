#pragma once

#include <dory/core/devices/iFileWatcherDevice.h>
#include <efsw/efsw.hpp>
#include <dory/core/allocators.h>
#include <dory/generic/allocationResource.h>

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
        GlobalAllocatorType& _allocator;
        generic::memory::AllocationResource<efsw::FileWatcher, GlobalAllocatorType> _fileWatcher;

    public:
        explicit FileWatcherDevice(Registry& registry, GlobalAllocatorType& allocator);

        void handleFileAction(efsw::WatchID watchid, const std::string& dir,
                               const std::string& filename, efsw::Action action,
                               std::string oldFilename) override;

        void updateWatches(resources::DataContext& context) override;
        void connect(resources::DataContext& context) override;
        void disconnect(resources::DataContext& context) override;
    };
}
