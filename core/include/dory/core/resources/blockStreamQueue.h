#pragma once

#include <dory/core/resources/entities/stream.h>
#include <dory/core/resources/assetType.h>
#include <readerwriterqueue.h>

namespace dory::core::resources::entities
{
    struct ImageBlock: public Block
    {
        std::vector<unsigned char> data;
        AssetTypeName assetFileType;
        unsigned int width;
        unsigned int height;
        unsigned char components;
    };

    struct BlockStreamQueue: public BlockStream
    {
        moodycamel::ReaderWriterQueue<Block> queue {};
    };
}