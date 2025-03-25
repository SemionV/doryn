#pragma once

#include <any>
#include <map>
#include <string>
#include "interface.h"
#include <dory/core/resources/id.h>
#include <dory/core/resources/systemTypes.h>

namespace dory::core::events
{
    enum class KeyCode
    {
        Unknown,
        Escape,
        Return,
        Backspace,
        Terminate,
        Character,
        Up,
        Down,
        Left,
        Right,
        W,
        A,
        S,
        D
    };

    enum class KeyAction
    {
        Unknown,
        Press,
        Release,
        Repeat
    };

    struct ModificationKeysState
    {
        bool shiftKey {};
        bool ctrlKey {};
        bool altKey {};
        bool superKey {};
    };

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
        struct WindowEvent
        {
            resources::IdType windowId {};
            resources::WindowSystem windowSystem {};
        };

        struct Close: public WindowEvent
        {};

        struct Resize: public WindowEvent
        {
            unsigned int width {};
            unsigned int height {};
        };

        struct KeyboardEvent: public WindowEvent
        {
            KeyCode keyCode { KeyCode::Unknown };
            KeyAction action { KeyAction::Unknown };
            ModificationKeysState modKeysState {};
        };

        using Bundle = EventBufferBundle<Close, Resize, KeyboardEvent>;
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

    namespace scene
    {
        struct SceneObjectEvent
        {
            resources::IdType sceneId {};
            resources::EcsType ecsType {};
            resources::IdType objectId {};
        };

        struct LinearMovementComplete: public SceneObjectEvent
        {};

        struct RotationMovementComplete: public SceneObjectEvent
        {};

        using Bundle = EventBufferBundle<LinearMovementComplete, RotationMovementComplete>;
    }
}