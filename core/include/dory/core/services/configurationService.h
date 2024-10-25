#pragma once

#include <dory/core/services/iConfigurationService.h>
#include <dory/core/implementation.h>
#include <dory/serialization/objectVisitor.h>
#include <dory/core/resources/configurationRefl.h>
#include <dory/core/services/iLogService.h>
#include <dory/core/resources/logger.h>

namespace dory::core
{
    class Registry;
}

namespace dory::core::services
{
    struct ConfigurationSectionContext
    {
        IConfigurationService& configurationService;

        explicit ConfigurationSectionContext(IConfigurationService& configurationService):
                configurationService(configurationService)
        {}
    };

    template<typename T>
    concept IsRecursiveSectionMetadata = requires (T x)
    {
        requires std::is_base_of_v<resources::configuration::RecursiveSection, T>;
    };

    template<typename T>
    concept IsRecursiveSection = requires (T x)
    {
        { x.section };
        requires IsRecursiveSectionMetadata<std::decay_t<decltype(x.section)>>;
    };

    struct LoadConfigurationSectionObjectPolicy: public dory::serialization::DefaultObjectPolicy
    {
        template<typename TContext, typename T>
        requires (!IsRecursiveSection<T> && !IsRecursiveSectionMetadata<T>)
        inline static bool beginObject(T&& object, TContext& context)
        {
            //skip non-recursive section
            return true;
        }

        template<typename TContext, typename T>
        requires (IsRecursiveSectionMetadata<T>)
        inline static bool beginObject(T&& object, TContext& context)
        {
            //skip recursive section metadata itself
            return false;
        }

        template<typename T>
        requires IsRecursiveSection<T>
        inline static bool beginObject(T&& object, ConfigurationSectionContext& context)
        {
            //recursive section
            auto overrideWithFiles = std::vector<std::string>{ std::move(object.section.loadFrom) };
            for(const auto& settingsFile : overrideWithFiles)
            {
                context.configurationService.load(settingsFile, std::forward<T>(object));
            }

            return true;
        }
    };

    struct SaveConfigurationSectionObjectPolicy: public dory::serialization::DefaultObjectPolicy
    {
        template<typename TContext, typename T>
        requires (!IsRecursiveSection<T> && !IsRecursiveSectionMetadata<T>)
        inline static bool beginObject(T&& object, TContext& context)
        {
            //skip non-recursive section
            return true;
        }

        template<typename TContext, typename T>
        requires (IsRecursiveSectionMetadata<T>)
        inline static bool beginObject(T&& object, TContext& context)
        {
            //skip recursive section metadata itself
            return false;
        }

        template<typename T>
        requires IsRecursiveSection<T>
        inline static bool beginObject(T&& object, ConfigurationSectionContext& context)
        {
            //recursive section
            auto& saveTo = object.section.saveTo;
            if(!saveTo.empty())
            {
                context.configurationService.save(saveTo, object);
            }

            return true;
        }
    };

    struct LoadConfigurationSectionPolicies: public dory::serialization::VisitorDefaultPolicies
    {
        using ObjectPolicy = LoadConfigurationSectionObjectPolicy;
    };

    struct SaveConfigurationSectionPolicies: public dory::serialization::VisitorDefaultPolicies
    {
        using ObjectPolicy = SaveConfigurationSectionObjectPolicy;
    };

    template<typename TPolicy>
    class ConfigurationServiceRoot: public IConfigurationService
    {
    protected:
        Registry& _registry;

    public:
        explicit ConfigurationServiceRoot(Registry& registry):
            _registry(registry)
        {}
    };

    template<typename T, typename TPolicy, typename TState>
    class ConfigurationServiceGeneric: public implementation::ImplementationLevel<TPolicy, TState>
    {
    public:
        explicit ConfigurationServiceGeneric(Registry& registry):
            implementation::ImplementationLevel<TPolicy, TState>(registry)
        {}

        bool load(const std::filesystem::path& configurationPath, T& configuration) final
        {
            try
            {
                this->_registry.template get<ILogService, resources::Logger::Config>([&configurationPath](ILogService* logger)
                {
                    logger->information(fmt::format("load configuration from: {0}", configurationPath.string()));
                });

                //auto source = fileService.read(configurationPath);
                //serializationServiceBundle.deserialize(formatKeyConverter.getFormat(configurationPath), source, configuration);

                //load recursive sections
                auto context = ConfigurationSectionContext{ *this };
                dory::serialization::ObjectVisitor<LoadConfigurationSectionPolicies>::visit(configuration, context);

                return true;
            }
            catch(const std::exception& e)
            {
                this->_registry.template get<ILogService, resources::Logger::Config>([&e](ILogService* logger)
                {
                    logger->warning(fmt::format("cannot load configuration: {0}", e.what()));
                });
            }
            catch(...)
            {
                this->_registry.template get<ILogService, resources::Logger::Config>([](ILogService* logger)
                {
                    logger->warning(std::string_view("cannot load configuration: unknown exception type"));
                });
            }

            return false;
        }

        bool load(T& configuration) final
        {
            //load recursive sections
            auto context = ConfigurationSectionContext{ *this };
            dory::serialization::ObjectVisitor<LoadConfigurationSectionPolicies>::visit(configuration, context);

            return true;
        }

        bool save(const std::filesystem::path& configurationPath, const T& configuration) final
        {
            return true;
        }

        bool save(const T& configuration) final
        {
            return true;
        }
    };

    struct ConfigurationServicePolicy: implementation::ImplementationPolicy<implementation::ImplementationList<ConfigurationServiceGeneric>, ConfigurationServiceRoot>
    {};

    using ConfigurationService = implementation::Implementation<generic::TypeList<>, IConfigurationService::Types, ConfigurationServicePolicy>;
}
