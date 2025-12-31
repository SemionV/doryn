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
    class FunctionRef;

    template<class R, class... Args>
    class FunctionRef<R(Args...)>
    {
    public:
        FunctionRef() noexcept = default;

        // Construct from any callable lvalue (non-owning).
        template<class F>
        explicit FunctionRef(F& f) noexcept
            requires (!std::is_same_v<std::remove_cvref_t<F>, FunctionRef>)
                  && std::is_invocable_r_v<R, F&, Args...>
        {
            obj_ = static_cast<void*>(std::addressof(f));
            call_ = [](void* o, Args... as) -> R {
                return std::invoke(*static_cast<F*>(o), std::forward<Args>(as)...);
            };
        }

        // Construct from function pointer.
        explicit FunctionRef(R(*fp)(Args...)) noexcept
        {
            obj_ = reinterpret_cast<void*>(fp);
            call_ = [](void* o, Args... as) -> R {
                auto f = reinterpret_cast<R(*)(Args...)>(o);
                return f(std::forward<Args>(as)...);
            };
        }

        explicit operator bool() const noexcept { return call_ != nullptr; }

        R operator()(Args... as) const
        {
            return call_(obj_, std::forward<Args>(as)...);
        }

    private:
        void* obj_ = nullptr;
        R (*call_)(void*, Args...) = nullptr;
    };

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
                ::new (_heap.ptr) Fn(std::forward<F>(f));
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

    template<typename TSignature>
    class Function;

    template<typename TReturn, typename... TArgs>
    class Function<TReturn(TArgs...)>
    {
    public:
        using size_type = std::size_t;

    private:
        enum class StorageKind: unsigned char
        {
            empty,
            local,
            heap
        };

        struct VirtualTable
        {
            TReturn (*invoke)(const void*, TArgs...);
            void (*destroy)(Function&) noexcept;
            void (*move_to)(Function& src, Function& dst) noexcept;
        };

        struct HeapData
        {
            void* ptr = nullptr;
            size_type size = 0;
            size_type align = 0;
        };

        template<size_type STORAGE_SIZE, size_type STORAGE_ALIGN>
        struct LocalData
        {
            alignas(STORAGE_ALIGN)
            unsigned char data[STORAGE_SIZE];
        };

        static constexpr size_type STORAGE_SIZE = sizeof(HeapData);
        static constexpr size_type SBO_THRESHOLD = STORAGE_SIZE;
        static constexpr size_type DATA_ALIGN = alignof(std::max_align_t);

        //lays at the 0 offset of the object, so must be no waste of space
        union
        {
            HeapData _heapData;
            LocalData<STORAGE_SIZE, DATA_ALIGN> _localData;
        };
        std::pmr::memory_resource* _memResource = nullptr;
        const VirtualTable* _vtable = nullptr;
        StorageKind _storageKind = StorageKind::empty;

        [[nodiscard]] bool isHeapStorage() const noexcept
        {
            return _storageKind == StorageKind::heap;
        }

        void setHeapMode() noexcept
        {
            _storageKind = StorageKind::heap;
        }

        void setLocalMode() noexcept
        {
            _storageKind = StorageKind::local;
        }

        void* getData()
        {
            return isHeapStorage() ? _heapData.ptr : _localData.data;
        }

        [[nodiscard]] const void* getData() const
        {
            return isHeapStorage() ? _heapData.ptr : _localData.data;
        }

        template<class F>
        static constexpr bool fitsLocal() noexcept
        {
            // We require nothrow move if stored locally, so Function move stays noexcept.
            return sizeof(F) <= SBO_THRESHOLD
                && alignof(F) <= DATA_ALIGN
                && std::is_nothrow_move_constructible_v<F>;
        }

        template<class F>
        void emplace(F&& f)
        {
            using Fn = std::remove_cvref_t<F>;

            if constexpr (fitsLocal<Fn>())
            {
                setLocalMode();
                ::new (static_cast<void*>(_localData.data)) Fn(std::forward<F>(f));
            }
            else
            {
                setHeapMode();
                _heapData.size = sizeof(Fn);
                _heapData.align = alignof(Fn);
                _heapData.ptr = _memResource->allocate(_heapData.size, _heapData.align);
                ::new (_heapData.ptr) Fn(std::forward<F>(f));
            }

            _vtable = &getVTable<Fn>();
        }

        template<class Fn>
        static const VirtualTable& getVTable() noexcept
        {
            static const VirtualTable vt {
                // invoke
                [](const void* p, TArgs... args) -> TReturn {
                    return std::invoke(*static_cast<const Fn*>(p), std::forward<TArgs>(args)...);
                },
                // destroy
                [](Function& self) noexcept {
                    auto* p = static_cast<Fn*>(self.getData());
                    p->~Fn();
                    if(self.isHeapStorage())
                    {
                        self._memResource->deallocate(self._heapData.ptr, self._heapData.size, self._heapData.align);
                        self._heapData = {};
                    }
                },
                // move_to
                [](Function& src, Function& dst) noexcept {
                    dst._memResource = src._memResource;
                    dst._storageKind = src._storageKind;
                    dst._vtable = &getVTable<Fn>();

                    if (src._storageKind == StorageKind::heap)
                    {
                        dst._heapData = src._heapData;
                        src._heapData = {};
                    } else if (src._storageKind == StorageKind::local)
                    {
                        auto* s = static_cast<Fn*>(src.getData());
                        ::new (static_cast<void*>(dst._localData.data)) Fn(std::move(*s));
                        s->~Fn();
                    }

                    src._storageKind = StorageKind::empty;
                    src._vtable = nullptr;
                }
            };
            return vt;
        }

        void reset() noexcept
        {
            if (_vtable)
            {
                _vtable->destroy(*this);
                _vtable = nullptr;
            }
            _storageKind = StorageKind::empty;
        }

        void moveFrom(Function&& other) noexcept
        {
            if (!other._vtable) return;
            other._vtable->move_to(other, *this);
        }

    public:
        Function() noexcept = default;

        explicit Function(std::nullptr_t) noexcept
        {}

        Function(const Function&) = delete;
        Function& operator=(const Function&) = delete;

        Function(Function&& other) noexcept
        {
            moveFrom(std::move(other));
        }

        Function& operator=(Function&& other) noexcept
        {
            if (this != &other)
            {
                reset();
                moveFrom(std::move(other));
            }
            return *this;
        }

        ~Function()
        {
            reset();
        }

        template<class F>
        requires std::is_invocable_r_v<TReturn, F&, TArgs...>
        explicit Function(std::pmr::memory_resource* memResource, F&& f)
        {
            _memResource = memResource ? memResource : std::pmr::get_default_resource();
            emplace(std::forward<F>(f));
        }

        template<class F>
        requires std::is_invocable_r_v<TReturn, F&, TArgs...>
        explicit Function(F&& f)
        {
            _memResource = std::pmr::get_default_resource();
            emplace(std::forward<F>(f));
        }

        explicit operator bool() const noexcept
        {
            return _vtable;
        }

        TReturn operator()(TArgs... args) const
        {
            assert::inhouse(this->operator bool(), "Calling empty Function");
            return _vtable->invoke(getData(), std::forward<TArgs>(args)...);
        }
    };
}
