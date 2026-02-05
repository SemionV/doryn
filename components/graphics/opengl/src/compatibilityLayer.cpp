#include <dory/graphics/opengl/compatibilityLayer.h>

namespace dory::graphics::opengl
{
    CompatibilityLayer::CompatibilityLayer()
    {
        _caps.hasDSA45 = GLAD_GL_VERSION_4_5;
        _caps.hasBufferStorage = GLAD_GL_VERSION_4_4;
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

    void CompatibilityLayer::namedBufferStorage(const GLenum target, const GLuint buffer, const GLsizeiptr size,
        const void *data, const GLbitfield flags) const
    {
        if (_caps.hasBufferStorage)
        {
            // GL 4.4+ (Windows/Linux)
            glNamedBufferStorage(buffer, size, data, flags);
        }
        else
        {
            // GL 4.1 fallback (macOS)

            // Translate immutable-storage flags → usage hint
            GLenum usage = GL_STATIC_DRAW;

            if (flags & GL_DYNAMIC_STORAGE_BIT)
                usage = GL_DYNAMIC_DRAW;
            else if (flags & GL_MAP_WRITE_BIT)
                usage = GL_STREAM_DRAW;

            glBindBuffer(target, buffer);
            glBufferData(target, size, data, usage);
            glBindBuffer(target, 0);
        }
    }

    void* CompatibilityLayer::mapNamedBuffer(GLenum target, GLuint buffer, GLenum access) const
    {
        if (_caps.hasDSA45)
        {
            // GL 4.5 path
            return glMapNamedBuffer(buffer, access);
        }

        // GL 4.1 fallback: bind-to-edit
        glBindBuffer(target, buffer);
        void* ptr = glMapBuffer(target, access);
        glBindBuffer(target, 0);
        return ptr;
    }

    bool CompatibilityLayer::unmapNamedBuffer(GLenum target, GLuint buffer) const
    {
        if (_caps.hasDSA45)
        {
            // GL 4.5 path
            return glUnmapNamedBuffer(buffer) == GL_TRUE;
        }

        // GL 4.1 fallback: bind-to-edit
        glBindBuffer(target, buffer);
        const GLboolean ok = glUnmapBuffer(target);
        glBindBuffer(target, 0);
        return ok == GL_TRUE;
    }

    void CompatibilityLayer::namedBufferSubData(const GLenum target,
                                            const GLuint buffer,
                                            const GLintptr offset,
                                            const GLsizeiptr size,
                                            const void* data) const
    {
        if (_caps.hasDSA45)
        {
            // GL 4.5 path
            glNamedBufferSubData(buffer, offset, size, data);
            return;
        }

        // GL 4.1 fallback: bind-to-edit
        glBindBuffer(target, buffer);
        glBufferSubData(target, offset, size, data);
        glBindBuffer(target, 0);
    }

    void CompatibilityLayer::createVertexArray(GLuint* out) const
    {
        if (_caps.hasDSA45)
        {
            glCreateVertexArrays(1, out);
            return;
        }

        glGenVertexArrays(1, out);

        // Important on many implementations: make it "real" by binding once
        glBindVertexArray(*out);
        glBindVertexArray(0);
    }

    void CompatibilityLayer::vaoBindVertexBuffer(GLuint vao, GLuint bindingIndex,
                                            GLuint buffer, GLintptr offset, GLsizei stride) const
    {
        if (_caps.hasDSA45)
        {
            glVertexArrayVertexBuffer(vao, bindingIndex, buffer, offset, stride);
            return;
        }

        // 4.1 fallback: no separate binding points. We bind VAO and ARRAY_BUFFER.
        (void)bindingIndex; // not meaningful in 4.1 path
        glBindVertexArray(vao);
        glBindBuffer(GL_ARRAY_BUFFER, buffer);
        // offset/stride will be used by vaoSetAttribPointerCompat or glVertexAttribPointer later
        // so this function alone doesn't finish the job in 4.1.
        // Keep it for structural parity, but it's okay if you don't call it on 4.1.
    }

    void CompatibilityLayer::vaoAttribFormat(GLuint vao, GLuint attribIndex,
                                            GLint size, GLenum type, GLboolean normalized,
                                            GLuint relativeOffset) const
    {
        if (_caps.hasDSA45)
        {
            glVertexArrayAttribFormat(vao, attribIndex, size, type, normalized, relativeOffset);
            return;
        }

        // 4.1 fallback: format is set via glVertexAttribPointer, not separable.
        // Provide a higher-level helper (vaoSetAttribPointerCompat) instead.
        (void)vao; (void)attribIndex; (void)size; (void)type; (void)normalized; (void)relativeOffset;
    }

    void CompatibilityLayer::vaoAttribBinding(GLuint vao, GLuint attribIndex, GLuint bindingIndex) const
    {
        if (_caps.hasDSA45)
        {
            glVertexArrayAttribBinding(vao, attribIndex, bindingIndex);
            return;
        }

        // 4.1: no attrib/binding split
        (void)vao; (void)attribIndex; (void)bindingIndex;
    }

    void CompatibilityLayer::vaoEnableAttrib(GLuint vao, GLuint attribIndex) const
    {
        if (_caps.hasDSA45)
        {
            glEnableVertexArrayAttrib(vao, attribIndex);
            return;
        }

        glBindVertexArray(vao);
        glEnableVertexAttribArray(attribIndex);
    }

    void CompatibilityLayer::vaoSetElementBuffer(GLuint vao, GLuint elementBuffer) const
    {
        if (_caps.hasDSA45)
        {
            glVertexArrayElementBuffer(vao, elementBuffer);
            return;
        }

        glBindVertexArray(vao);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementBuffer); // this is VAO state
    }

    void CompatibilityLayer::vaoSetAttribPointerCompat(GLuint vao,
                                                       GLuint attribIndex,
                                                       GLuint vbo,
                                                       GLint size, GLenum type, GLboolean normalized,
                                                       GLsizei stride,
                                                       GLintptr absoluteOffset) const
    {
        if (_caps.hasDSA45)
        {
            // For DSA we can emulate this "one-shot" call using your existing split calls.
            // Here we assume bindingIndex == attribIndex (your current pattern).
            const GLuint bindingIndex = attribIndex;
            glVertexArrayVertexBuffer(vao, bindingIndex, vbo, 0, stride);
            glVertexArrayAttribFormat(vao, attribIndex, size, type, normalized,
                                      static_cast<GLuint>(absoluteOffset)); // relative offset within bound buffer range
            glVertexArrayAttribBinding(vao, attribIndex, bindingIndex);
            glEnableVertexArrayAttrib(vao, attribIndex);
            return;
        }

        // 4.1 fallback: classic VAO setup
        glBindVertexArray(vao);
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glVertexAttribPointer(attribIndex, size, type, normalized, stride,
                              reinterpret_cast<const void*>(absoluteOffset));
        glEnableVertexAttribArray(attribIndex);
    }
}
