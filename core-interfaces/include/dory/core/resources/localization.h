#pragma once

#include <string>
#include <memory>
#include <dory/generic/parameterizedString.h>
#include <dory/generic/baseTypes.h>

namespace dory::core::resources
{
    struct Localization
    {
    protected:
        class IGoodByeTemplate: public generic::Interface
        {
        public:
            virtual std::string get(const std::string& name) = 0;
        };

        class IBirthDateTemplate: public generic::Interface
        {
        public:
            virtual std::string get(unsigned int day, unsigned int month, unsigned int year) = 0;
        };

    public:
        std::string hello;
        std::unique_ptr<IGoodByeTemplate> goodBye;
        std::unique_ptr<IBirthDateTemplate> birthDate;
    };
}
