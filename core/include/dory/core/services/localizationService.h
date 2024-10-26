#pragma once

#include <dory/core/services/iLocalizationService.h>

namespace dory::core
{
    class Registry;
}

namespace dory::core::services
{

    class LocalizationService: public ILocalizationService
    {
    private:
        Registry& _registry;

    public:
        explicit LocalizationService(Registry& registry);

        void load(const resources::configuration::Configuration& configuration, resources::Localization& localization) override;
    };
}
