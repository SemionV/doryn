#include <dory/graphics/opengl/compatibilityLayer.h>

namespace dory::graphics::opengl
{
    CompatibilityLayer::CompatibilityLayer()
    {
        _caps.hasDSA45 = GLAD_GL_VERSION_4_5;
    }

    void CompatibilityLayer::createBuffer(GLuint *out) const
    {
        if(_caps.hasDSA45)
        {
            glCreateBuffers(1, out);
        }
        else
        {
            glGenBuffers(1, out);
        }
    }
}
