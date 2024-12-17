#pragma once

#include <any>
#include <map>
#include <string>
#include "interface.h"
#include <dory/core/resources/id.h>
#include <dory/core/resources/windowSystem.h>

namespace dory::core::events
{
    struct pipeline
    {
        struct Initialize {};
        struct Stop {};

        using Bundle = EventBundle<const Initialize, const Stop>;
    };

    namespace application
    {
        struct Exit {};

        using Bundle = EventBundle<const Exit>;
    }

    namespace io
    {
        enum class KeyCode
        {
            Unknown,
            Escape,
            Return,
            Backspace,
            Terminate,
            Character
        };

        struct KeyPressEvent
        {
            KeyCode keyCode = KeyCode::Unknown;
            int character = 0;
        };

        using Bundle = EventBufferBundle<KeyPressEvent>;
    }

    namespace script
    {
        struct Run
        {
            const std::string scriptKey;
            const std::map<std::string, std::any> arguments;
        };

        using Bundle = EventBundle<const Run>;
    }

    namespace window
    {
        struct Close
        {
            resources::IdType windowId;
            resources::WindowSystem windowSystem;
        };

        struct Resize
        {
            resources::IdType windowId;
            unsigned int width;
            unsigned int height;
            resources::WindowSystem windowSystem;
        };

        using Bundle = EventBufferBundle<Close, Resize>;
    }

    namespace filesystem
    {
        struct FileEvent
        {
            std::filesystem::path filePath {};
        };

        struct FileModified: public FileEvent
        {};

        using Bundle = EventBufferBundle<FileModified>;
    }
}