#pragma once

#include <refl.hpp>
#include <dory/generics/typeTraits.h>

#include "stringResource.h"

namespace dory::engine::resources::localization
{
    struct Localization
    {
        std::string hello;

        struct GoodByeParameterizedString: public ParameterizedString {
            template<typename T>
            std::string get(T&& name) {
                return format(std::forward<T>(name));
            }
        };
        GoodByeParameterizedString goodBye;

        struct BirthDateParameterizedString: public ParameterizedString {
            std::string get(unsigned int day, unsigned int month, unsigned int year) {
                return format(day, month, year);
            }
        };
        BirthDateParameterizedString birthDate;
    };
}
REFL_TYPE(dory::engine::resources::localization::Localization)
    REFL_FIELD(hello)
    REFL_FIELD(goodBye)
    REFL_FIELD(birthDate)
REFL_END