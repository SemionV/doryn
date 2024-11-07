#pragma once

namespace dory::generic
{
    template<typename T>
    class SpanIterator
    {
    private:
        std::size_t _size {};
        T* _data = nullptr;

    public:
        using value_type = T;
        using difference_type = std::size_t;
        using reference = T&;
        using pointer = T*;
        using iterator_category = std::forward_iterator_tag;

        SpanIterator() = default;

        explicit SpanIterator(T* data, std::size_t size):
            _data(data), _size(size)
        {}

        reference operator*() const {
            return *_data;
        }

        pointer operator->() const {
            return _data;
        }

        SpanIterator& operator++() {
            _data = _data + _size;
            return *this;
        }

        SpanIterator operator++(int) {
            SpanIterator temp = *this;
            ++(*this);
            return temp;
        }

        bool operator==(const SpanIterator& other) const {
            return _data == other._data;
        }

        bool operator!=(const SpanIterator& other) const {
            return _data != other._data;
        }
    };

    template<typename T>
    class Span
    {
    private:
        std::size_t _size;
        std::size_t _count;
        T* _data;

    public:
        explicit Span(T* data, std::size_t count, std::size_t size = sizeof(T)):
                _data(data), _count(count), _size(size)
        {}

        SpanIterator<T> begin()
        {
            return SpanIterator<T>{ _data, _size };
        }

        SpanIterator<T> end()
        {
            return SpanIterator<T>{ _data + _count * _size, _size };
        }
    };
}
