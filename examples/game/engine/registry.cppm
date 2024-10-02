module;

#include <dory/core/macro.h>
#include <string>

export module game.engine.registry;

namespace dory::game::engine
{
    export std::string DORY_DLLEXPORT getMessage();
}