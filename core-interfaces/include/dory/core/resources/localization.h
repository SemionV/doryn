#pragma once

#include <string>
#include <memory>
#include <dory/generic/parameterizedString.h>

namespace dory::core::resources
{
    struct Localization
    {
    public:
        class IGoodByeTemplate: public generic::IParameterizedString
        {
        public:
            virtual std::string get(const std::string& name) = 0;
        };

        class IBirthDateTemplate: public generic::IParameterizedString
        {
        public:
            virtual std::string get(unsigned int day, unsigned int month, unsigned int year) = 0;
        };

    public:
        std::string hello;
        IGoodByeTemplate* goodBye;
        IBirthDateTemplate* birthDate;
    };
}
