#pragma once

#include <dory/core/resources/entities/stream.h>
#include <readerwriterqueue.h>

namespace dory::core::resources::entities
{
    struct BlockStreamQueue: public BlockStream
    {
        moodycamel::ReaderWriterQueue<Block> queue {};
    };
}
