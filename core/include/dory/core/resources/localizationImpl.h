#pragma once

#include <dory/core/resources/localization.h>
#include "parameterizedString.h"
#include <dory/core/allocators.h>

namespace dory::core::resources
{
    template<typename TAllocator>
    struct LocalizationImpl: public Localization
    {
    private:
        TAllocator& _allocator;

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
        explicit LocalizationImpl(TAllocator& allocator):
        _allocator(allocator)
        {
            goodBye = static_cast<IGoodByeTemplate*>(_allocator.allocate(sizeof(GoodByeTemplate)));
            birthDate = static_cast<IBirthDateTemplate*>(_allocator.allocate(sizeof(BirthDateTemplate)));
        }

        ~LocalizationImpl()
        {
            _allocator.deallocate(goodBye);
            _allocator.deallocate(birthDate);
        }
    };
}