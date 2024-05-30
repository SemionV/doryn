#pragma once

#include "dependencies.h"

namespace dory
{
    using Byte = std::uint8_t;

    template <class, template <class, class...> class>
    struct is_instance : public std::false_type {};

    template <class...Ts, template <class, class...> class U>
    struct is_instance<U<Ts...>, U> : public std::true_type {};

    template<typename T, template <class, class...> class U>
    constexpr bool is_instance_v = is_instance<T, U>::value;

    template<class A>
    struct is_fixed_array: std::false_type {};

    template<class T, std::size_t I>
    struct is_fixed_array<std::array<T, I>>: std::true_type {};

    template<class T>
    constexpr bool is_fixed_array_v = is_fixed_array<T>::value;

    template<class A>
    struct is_vector: std::false_type {};

    template<class T>
    struct is_vector<std::vector<T>>: std::true_type {};

    template<class T>
    constexpr bool is_vector_v = is_vector<T>::value;

    template<class A>
    struct is_deque: std::false_type {};

    template<class T>
    struct is_deque<std::deque<T>>: std::true_type {};

    template<class T>
    constexpr bool is_deque_v = is_deque<T>::value;

    template<class T>
    constexpr bool is_dynamic_collection_v = is_deque_v<T> || is_vector_v<T>;

    template<class T>
    struct is_map: std::false_type {};

    template<class K, class T>
    struct is_map<std::map<K, T>>: std::true_type {};

    template<class T>
    constexpr bool is_map_v = is_map<T>::value;

    template<class T>
    struct is_unordered_map: std::false_type {};

    template<class K, class T>
    struct is_unordered_map<std::unordered_map<K, T>>: std::true_type {};

    template<class T>
    constexpr bool is_unordered_map_v = is_unordered_map<T>::value;

    template<class T>
    constexpr bool is_dictionary_v = is_unordered_map_v<T> || is_map_v<T>;

    template<class A>
    struct is_optional: std::false_type {};

    template<class T>
    struct is_optional<std::optional<T>>: std::true_type {};

    template<class T>
    constexpr bool is_optional_v = is_optional<T>::value;

    template<class T>
    struct is_pair: std::false_type {};

    template<class K, class T>
    struct is_pair<std::pair<K, T>>: std::true_type {};

    template<class T>
    constexpr bool is_pair_v = is_pair<T>::value;

    template<typename>
    struct array_size;

    template<typename T, size_t N>
    struct array_size<std::array<T,N>>
    {
        static size_t const size = N;
    };

    template<class T>
    struct CollectionValueType;

    template<class T>
    requires(is_dynamic_collection_v<T>)
    struct CollectionValueType<T>
    {
        using Type = typename T::value_type;
    };

    template<class T>
    requires(is_dictionary_v<T>)
    struct CollectionValueType<T>
    {
        using Type = typename T::mapped_type;
    };

    template<class T>
    using CollectionValueTypeType = CollectionValueType<T>::Type;

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

    template<typename T>
    using OptionalReference = std::optional<std::reference_wrapper<T>>;

    template<class T>
    OptionalReference<T> makeOptionalRef(T& ref)
    {
        return OptionalReference<T>{std::ref(ref)};
    }

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

    template<typename TImpelementation>
    TImpelementation* toImplementation(void* implementation)
    {
        return static_cast<TImpelementation*>(implementation);
    }

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