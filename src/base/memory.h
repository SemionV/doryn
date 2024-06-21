#pragma once

#include "base/dependencies.h"

namespace dory::memory
{
    template <typename T>
    class CustomAllocator {
    public:
        // Allocate memory
        T* allocate(std::size_t n) {
            std::cout << "Allocating " << n << " objects.\n";
            return static_cast<T*>(::operator new(n * sizeof(T)));
        }

        // Deallocate memory
        void deallocate(T* ptr, std::size_t n) {
            std::cout << "Deallocating " << n << " objects.\n";
            ::operator delete(ptr);
        }

        // Construct an object in allocated memory
        template <typename... Args>
        void construct(T* ptr, Args&&... args) {
            new (ptr) T(std::forward<Args>(args)...);
        }

        // Destroy an object in allocated memory
        void destroy(T* ptr) {
            ptr->~T();
        }
    };

// Custom deleter that uses the custom allocator
    template <typename T>
    class CustomDeleter {
    public:
        CustomDeleter(CustomAllocator<T>* allocator) : allocator_(allocator) {}

        void operator()(T* ptr) const {
            allocator_->destroy(ptr);
            allocator_->deallocate(ptr, 1);
        }

    private:
        CustomAllocator<T>* allocator_;
    };
}