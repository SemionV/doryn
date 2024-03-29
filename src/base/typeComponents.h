#pragma once

#include "dependencies.h"

namespace dory
{
    template <class, template <class, class...> class>
    struct is_instance : public std::false_type {};

    template <class...Ts, template <class, class...> class U>
    struct is_instance<U<Ts...>, U> : public std::true_type {};

    template<typename T, template <class, class...> class U>
    constexpr bool is_instance_v = is_instance<T, U>::value;

    constexpr std::size_t getStringLength(const char* string)
    {
        std::size_t length = 0;
        if(string)
        {
            while(string[length] != 0)
            {
                ++length;
            }
        }

        return length;
    }

    template <std::string_view const&... Strs>
    struct JoinStrings
    {
        static constexpr auto impl() noexcept
        {
            constexpr std::size_t len = (Strs.size() + ... + 0);
            std::array<char, len + 1> arr{};
            auto append = [i = 0, &arr](auto const& s) mutable {
                for (auto c : s) arr[i++] = c;
            };
            (append(Strs), ...);
            arr[len] = 0;
            return arr;
        }
        static constexpr auto arr = impl();
        static constexpr std::string_view value {arr.data(), arr.size() - 1};
    };

    template <std::string_view const&... Strs>
    static constexpr auto JoinStringLiterals = JoinStrings<Strs...>::value;

    class Uncopyable
    {
    public:
        Uncopyable(const Uncopyable&) = delete;
        Uncopyable& operator=(const Uncopyable&) = delete;

        Uncopyable() = default;
    };

    template<typename TImpelementation>
    class StaticInterface
    {
    protected:
        TImpelementation* toImplementation()
        {
            return static_cast<TImpelementation*>(this);
        }
    };

    template<typename TImplementation>
    class IServiceFactory: Uncopyable, public StaticInterface<TImplementation>
    {
    public:
        auto createInstance()
        {
            return this->toImplementation()->createInstanceImpl();
        }
    };

    template<typename TService, typename TServiceInterface = TService>
    class ServiceFactory: public IServiceFactory<ServiceFactory<TService, TServiceInterface>>
    {
    public:
        TServiceInterface createInstanceImpl()
        {
            return TService{};
        }
    };

    template<typename TService, typename TServiceInterface>
    class ServiceFactory<std::shared_ptr<TService>, TServiceInterface>:
            public IServiceFactory<ServiceFactory<std::shared_ptr<TService>, TServiceInterface>>
    {
    public:
        std::shared_ptr<TServiceInterface> createInstanceImpl()
        {
            return std::static_pointer_cast<TServiceInterface>(std::make_shared<TService>());
        }
    };
}