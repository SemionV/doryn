#pragma once

#include <refl.hpp>
#include <dory/core/resources/configuration.h>

REFL_TYPE(dory::core::resources::configuration::RecursiveSection)
    REFL_FIELD(loadFrom)
    REFL_FIELD(saveTo)
    REFL_FIELD(description)
REFL_END

REFL_TYPE(dory::core::resources::configuration::RotationLogSink)
    REFL_FIELD(logFileName)
    REFL_FIELD(maximumFileSize)
    REFL_FIELD(maximumFilesCount)
REFL_END

REFL_TYPE(dory::core::resources::configuration::StdoutLogSink)
REFL_END

REFL_TYPE(dory::core::resources::configuration::Logger)
    REFL_FIELD(name)
    REFL_FIELD(rotationLogger)
    REFL_FIELD(stdoutLogger)
REFL_END

REFL_TYPE(dory::core::resources::configuration::LoggingConfiguration)
    REFL_FIELD(mainLogger)
    REFL_FIELD(configurationLogger)
REFL_END

REFL_TYPE(dory::core::resources::configuration::ShaderLoader)
        REFL_FIELD(shadersDirectory)
REFL_END

REFL_TYPE(dory::core::resources::configuration::Interface)
    REFL_FIELD(section)
    REFL_FIELD(activeLanguage)
REFL_END

REFL_TYPE(dory::core::resources::configuration::BuildInfo)
        REFL_FIELD(commitSHA)
        REFL_FIELD(timestamp)
        REFL_FIELD(version)
REFL_END

REFL_TYPE(dory::core::resources::configuration::FilesSystemWatch)
        REFL_FIELD(directory)
        REFL_FIELD(recursive)
REFL_END

REFL_TYPE(dory::core::resources::configuration::Extension)
        REFL_FIELD(name)
        REFL_FIELD(path)
        REFL_FIELD(reloadOnModification)
        REFL_FIELD(modules)
REFL_END

REFL_TYPE(dory::core::resources::configuration::Shader)
        REFL_FIELD(filename)
        REFL_FIELD(type)
REFL_END

REFL_TYPE(dory::core::resources::configuration::Program)
        REFL_FIELD(key)
        REFL_FIELD(shaders)
REFL_END

REFL_TYPE(dory::core::resources::configuration::RenderingMaterial)
        REFL_FIELD(program)
REFL_END

REFL_TYPE(dory::core::resources::configuration::Configuration)
    REFL_FIELD(section)
    REFL_FIELD(buildInfo)
    REFL_FIELD(localizations)
    REFL_FIELD(loggingConfiguration)
    REFL_FIELD(shaderLoader)
    REFL_FIELD(userInterface)
    REFL_FIELD(fileSystemWatches)
    REFL_FIELD(extensions)
    REFL_FIELD(assetTypes)
    REFL_FIELD(materials)
REFL_END