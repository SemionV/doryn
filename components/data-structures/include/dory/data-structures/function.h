#pragma once
#include <utility>
#include <memory_resource>
#include <type_traits>
#include <dory/macros/assert.h>

namespace dory::data_structures::function
{
    template<class R, class C, class... Args>
    auto bindMember(C* obj, R(C::*pmf)(Args...))
    {
        return [obj, pmf](Args... args) -> R {
            return std::invoke(pmf, obj, std::forward<Args>(args)...);
        };
    }

    template<class R, class C, class... Args>
    auto bindMember(const C* obj, R(C::*pmf)(Args...) const)
    {
        return [obj, pmf](Args... args) -> R {
            return std::invoke(pmf, obj, std::forward<Args>(args)...);
        };
    }

    template<class>
    class UniqueFunction;

    template<class R, class... Args>
    class UniqueFunction<R(Args...)>
    {
        // Tune SBO size for your engine; 3 pointers is a common sweet spot.
        static constexpr std::size_t inline_size  = 3 * sizeof(void*);
        static constexpr std::size_t inline_align = alignof(std::max_align_t);

        struct vtable
        {
            R (*invoke)(const void*, Args&&...);
            void (*destroy)(UniqueFunction&) noexcept;
            void (*move_to)(UniqueFunction& src, UniqueFunction& dst) noexcept;
        };

        enum class storage_kind : unsigned char { empty, inl, heap };

        struct heap_block
        {
            void* ptr = nullptr;
            std::size_t bytes = 0;
            std::size_t align = 0;
        };

        alignas(inline_align) unsigned char _inl[inline_size]{};
        heap_block _heap{};
        std::pmr::memory_resource* _mr = nullptr;
        const vtable* _vt = nullptr;
        storage_kind _storage_kind = storage_kind::empty;

    public:
        UniqueFunction() noexcept = default;

        explicit UniqueFunction(std::nullptr_t) noexcept {}

        UniqueFunction(const UniqueFunction&) = delete;
        UniqueFunction& operator=(const UniqueFunction&) = delete;

        UniqueFunction(UniqueFunction&& other) noexcept
        {
            move_from(std::move(other));
        }

        UniqueFunction& operator=(UniqueFunction&& other) noexcept
        {
            if (this != &other)
            {
                reset();
                move_from(std::move(other));
            }
            return *this;
        }

        ~UniqueFunction() { reset(); }

        // Construct with explicit memory_resource (recommended for engine subsystems)
        template<class F>
        explicit UniqueFunction(std::pmr::memory_resource* mr, F&& f)
            requires std::is_invocable_r_v<R, F&, Args...>
        {
            emplace_impl(mr ? mr : std::pmr::get_default_resource(), std::forward<F>(f));
        }

        // Construct using default resource
        template<class F>
        explicit UniqueFunction(F&& f)
            requires std::is_invocable_r_v<R, F&, Args...>
        {
            emplace_impl(std::pmr::get_default_resource(), std::forward<F>(f));
        }

        void reset() noexcept
        {
            if (_vt) _vt->destroy(*this);
            _vt = nullptr;
            _mr = nullptr;
            _storage_kind = storage_kind::empty;
            _heap = {nullptr, 0, 0};
        }

        explicit operator bool() const noexcept { return _vt != nullptr; }

        R operator()(Args... args) const
        {
            assert::inhouse(_vt, "calling empty UniqueFunction");
            return _vt->invoke(data_ptr(), std::forward<Args>(args)...);
        }

        [[nodiscard]] std::pmr::memory_resource* resource() const noexcept { return _mr; }

    private:
        void* data_ptr() noexcept
        {
            return _storage_kind == storage_kind::heap ? _heap.ptr : static_cast<void*>(_inl);
        }
        [[nodiscard]] const void* data_ptr() const noexcept
        {
            return _storage_kind == storage_kind::heap ? _heap.ptr : static_cast<const void*>(_inl);
        }

        template<class F>
        static constexpr bool fits_inline() noexcept
        {
            // We require nothrow move if stored inline, so unique_function move stays noexcept.
            return sizeof(F) <= inline_size
                && alignof(F) <= inline_align
                && std::is_nothrow_move_constructible_v<F>;
        }

        template<class F>
        void emplace_impl(std::pmr::memory_resource* mr, F&& f)
        {
            using Fn = std::remove_cvref_t<F>;
            _mr = mr;

            if constexpr (fits_inline<Fn>())
            {
                _storage_kind = storage_kind::inl;
                ::new (static_cast<void*>(_inl)) Fn(std::forward<F>(f));
                _vt = &inline_vtable<Fn>();
            }
            else
            {
                _storage_kind = storage_kind::heap;
                _heap.bytes = sizeof(Fn);
                _heap.align = alignof(Fn);
                _heap.ptr = _mr->allocate(_heap.bytes, _heap.align);
                try
                {
                    ::new (_heap.ptr) Fn(std::forward<F>(f));
                }
                catch (...)
                {
                    _mr->deallocate(_heap.ptr, _heap.bytes, _heap.align);
                    _heap = {};
                    _storage_kind = storage_kind::empty;
                    _mr = nullptr;
                    throw;
                }
                _vt = &heap_vtable<Fn>();
            }
        }

        void move_from(UniqueFunction&& other) noexcept
        {
            if (!other._vt) return;
            other._vt->move_to(other, *this);
            other.reset();
        }

        template<class Fn>
        static const vtable& inline_vtable() noexcept
        {
            static const vtable vt{
                // invoke
                [](const void* p, Args&&... as) -> R {
                    return std::invoke(*static_cast<const Fn*>(p), std::forward<Args>(as)...);
                },
                // destroy
                [](UniqueFunction& self) noexcept {
                    auto* p = static_cast<Fn*>(self.data_ptr());
                    p->~Fn();
                },
                // move_to
                [](UniqueFunction& src, UniqueFunction& dst) noexcept {
                    dst._mr = src._mr;
                    dst._storage_kind = storage_kind::inl;
                    ::new (static_cast<void*>(dst._inl)) Fn(std::move(*static_cast<Fn*>(src.data_ptr())));
                    dst._vt = &inline_vtable<Fn>();
                }
            };
            return vt;
        }

        template<class Fn>
        static const vtable& heap_vtable() noexcept
        {
            static const vtable vt{
                // invoke
                [](const void* p, Args&&... as) -> R {
                    return std::invoke(*static_cast<const Fn*>(p), std::forward<Args>(as)...);
                },
                // destroy
                [](UniqueFunction& self) noexcept {
                    auto* p = static_cast<Fn*>(self._heap.ptr);
                    p->~Fn();
                    self._mr->deallocate(self._heap.ptr, self._heap.bytes, self._heap.align);
                },
                // move_to (no allocations, just pointer transfer)
                [](UniqueFunction& src, UniqueFunction& dst) noexcept {
                    dst._mr = src._mr;
                    dst._storage_kind = storage_kind::heap;
                    dst._heap = src._heap;
                    dst._vt = &heap_vtable<Fn>();

                    src._heap = {}; // prevent double free in src.reset()
                }
            };
            return vt;
        }
    };
}
