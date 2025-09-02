#pragma once

#include <cstddef>
#include <new>

void* operator new(std::size_t sz);
void* operator new(std::size_t sz, const std::nothrow_t&) noexcept;
void* operator new[](std::size_t sz);
void* operator new[](std::size_t sz, const std::nothrow_t&) noexcept;
void* operator new(std::size_t sz, std::align_val_t al);
void* operator new(std::size_t sz, std::align_val_t al, const std::nothrow_t&) noexcept;
void* operator new[](std::size_t sz, std::align_val_t al);
void* operator new[](std::size_t sz, std::align_val_t al, const std::nothrow_t&) noexcept;

void operator delete(void* ptr) noexcept;
void operator delete(void* ptr, std::size_t) noexcept;
void operator delete(void* ptr, const std::nothrow_t&) noexcept;
void operator delete[](void* ptr) noexcept;
void operator delete[](void* ptr, std::size_t) noexcept;
void operator delete[](void* ptr, const std::nothrow_t&) noexcept;
void operator delete(void* ptr, std::align_val_t) noexcept;
void operator delete(void* ptr, std::size_t, std::align_val_t) noexcept;
void operator delete(void* ptr, std::align_val_t, const std::nothrow_t&) noexcept;
void operator delete[](void* ptr, std::align_val_t) noexcept;
void operator delete[](void* ptr, std::size_t, std::align_val_t) noexcept;
void operator delete[](void* ptr, std::align_val_t, const std::nothrow_t&) noexcept;