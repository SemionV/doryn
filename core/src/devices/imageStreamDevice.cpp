#include <dory/core/registry.h>
#include <dory/core/devices/imageStreamDevice.h>
#include <thread>
#include <dory/core/resources/imageStreamQueue.h>

#include "dory/profiling/profiler.h"

namespace dory::core::devices
{
    void deleteOldImages(const std::filesystem::path& directory, const std::size_t maxImagesCount)
    {
        std::vector<std::filesystem::directory_entry> files;
        for (const auto& entry : std::filesystem::directory_iterator(directory))
        {
            if (entry.is_regular_file())
            {
                files.emplace_back(entry);
            }
        }

        if (files.size() > maxImagesCount)
        {
            // Sort the files by creation time (oldest first)
            std::ranges::sort(files, [](const std::filesystem::directory_entry& a, const std::filesystem::directory_entry& b) {
                return std::filesystem::last_write_time(a) < std::filesystem::last_write_time(b);
            });

            // Remove the oldest files
            for (size_t i = 0; i < files.size() - maxImagesCount; ++i)
            {
                std::filesystem::remove(files[i].path());
            }
        }
    }

    ImageStreamDevice::ImageStreamDevice(Registry& registry): DependencyResolver(registry)
    {}

    void ImageStreamDevice::connect(resources::DataContext& context)
    {
        if(stop)
        {
            stop = false;
            auto thread = std::jthread{ [this] { workingThread(); } };
            thread.detach();
        }
    }

    void ImageStreamDevice::disconnect(resources::DataContext& context)
    {
        stop = true;
        updateStream();
    }

    void ImageStreamDevice::updateStream()
    {
        queueUpdate.notify_one();
    }

    void ImageStreamDevice::workingThread()
    {
        DORY_TRACE_THREAD_NAME("dory::image_stream");

        while(!stop)
        {
            auto imageStreamRepo = _registry.get<repositories::IImageStreamRepository>();

            if(imageStreamRepo)
            {
                imageStreamRepo->each([this](resources::entities::ImageStream& stream)
                {
                    if (std::filesystem::exists(stream.destinationDirectory) && std::filesystem::is_directory(stream.destinationDirectory))
                    {
                        auto& streamQueue = static_cast<resources::entities::ImageStreamQueue&>(stream);
                        if(auto imageFileService = _registry.get<services::files::IImageFileService>(streamQueue.fileFormat))
                        {
                            resources::assets::Image image;
                            if(streamQueue.receive(image))
                            {
                                //TODO: use asynchronous method
                                imageFileService->save(stream.destinationDirectory / image.name, image);
                                deleteOldImages(stream.destinationDirectory, streamQueue.maxImagesCount);
                            }
                        }
                    }
                });
            }

            std::unique_lock lock { queueMutex };
            queueUpdate.wait(lock, [this] {
                return stop || isWorkAvailable();
            });
        }
    }

    bool ImageStreamDevice::isWorkAvailable() const
    {
        if(auto blockStreamRepo = _registry.get<repositories::IImageStreamRepository>())
        {
            const auto nonEmptyBlockStream = blockStreamRepo->scan([this](resources::entities::ImageStream& stream)
            {
                auto& streamQueue = static_cast<resources::entities::ImageStreamQueue&>(stream);
                return streamQueue.empty();
            });

            return nonEmptyBlockStream != nullptr;
        }

        return false;
    }

}
