#pragma once

#include <dory/core/resources/localization.h>
#include "parameterizedString.h"

namespace dory::core::resources
{
    struct LocalizationImpl: public Localization
    {
    private:
        class GoodByeTemplate: public ParameterizedString<IGoodByeTemplate> {
        public:
            std::string get(const std::string& name) final {
                return format(name);
            }
        };

        class BirthDateTemplate: public ParameterizedString<IBirthDateTemplate> {
        public:
            std::string get(unsigned int day, unsigned int month, unsigned int year) final {
                return format(day, month, year);
            }
        };

    public:
        LocalizationImpl()
        {
            goodBye = std::make_unique<GoodByeTemplate>();
            birthDate = std::make_unique<BirthDateTemplate>();
        }
    };
}