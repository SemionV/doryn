#pragma once

#ifndef DORY_OPENGL_INCLUDED
#include <glad/gl.h>
#define DORY_OPENGL_INCLUDED
#endif

#include "caps.h"

namespace dory::graphics::opengl
{
    class CompatibilityLayer
    {
    private:
        GLCaps _caps;
    public:
        CompatibilityLayer();

        void createBuffer(GLuint* out) const;
    };
}