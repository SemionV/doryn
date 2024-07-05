#include <dory/engine.h>

namespace dory::opengl
{
    graphics::Buffer allocateBuffer(GLint bufferSize)
    {
        graphics::Buffer buffer;
        buffer.data = malloc(bufferSize);
        if(buffer.data != nullptr)
        {
            buffer.size = bufferSize;
        }

        return buffer;
    }

    void deallocateBuffer(graphics::Buffer& buffer) noexcept
    {
        if(buffer.data)
        {
            free(buffer.data);
            buffer.data = 0;
            buffer.size = 0;
        }
    }
}