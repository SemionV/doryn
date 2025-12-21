#pragma once

#include <dory/core/resources/localization.h>
#include "parameterizedString.h"
#include <dory/generic/allocationResource.h>

namespace dory::core::resources
{
    template<typename TAllocator>
    struct LocalizationImpl: public Localization
    {
    private:
        class GoodByeTemplate: public ParameterizedString<IGoodByeTemplate> {
        public:
            std::string get(const std::string& name) final
            {
                return format(name);
            }
        };

        class BirthDateTemplate: public ParameterizedString<IBirthDateTemplate> {
        public:
            std::string get(unsigned int day, unsigned int month, unsigned int year) final {
                return format(day, month, year);
            }
        };

        generic::memory::AllocationResource<GoodByeTemplate, TAllocator> _goodByeTemplate;
        generic::memory::AllocationResource<BirthDateTemplate, TAllocator> _birthDateTemplate;

    public:
        explicit LocalizationImpl(TAllocator& allocator):
        _goodByeTemplate(allocator),
        _birthDateTemplate(allocator)
        {
            goodBye = _goodByeTemplate.get();
            birthDate = _birthDateTemplate.get();
        }
    };
}