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
