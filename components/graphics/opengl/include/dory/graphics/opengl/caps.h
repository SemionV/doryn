#pragma once

namespace dory::graphics::opengl
{
    struct GLCaps
    {
        bool hasDSA45 = false; // OpenGL 4.5 core DSA(Direct State Access) entrypoints
        bool hasBufferStorage = false;
    };
}