#pragma once

#include <dory/macros/assert.h>
#include <dory/bitwise/numbers.h>
#include <functional>
#include <utility>
#include <initializer_list>
#include <iterator>
#include <cmath>
#include <cstring>

namespace dory::containers
{
    //General purpose Hash Map, which is a replica of std::unordered_map from standard library
    //Main difference is awareness about the alocator
    template<
        typename Key,
        typename T,
        typename TAllocator,
        typename Hash = std::hash<Key>,
        typename KeyEqual = std::equal_to<Key>
    >
    class HashMap
    {
    public:
        // ------------------------------------------------------------
        // Member types
        // ------------------------------------------------------------
        using key_type        = Key;
        using mapped_type     = T;
        using value_type      = std::pair<const Key, T>;
        using hasher          = Hash;
        using key_equal       = KeyEqual;
        using allocator_type  = TAllocator;

        using size_type       = std::size_t;
        using difference_type = std::ptrdiff_t;

        // Node-based container → node handles store value_type
        // Define later: struct Node;
        class iterator;       // forward iterator
        class const_iterator;

        using local_iterator        = iterator;        // for bucket iteration
        using const_local_iterator  = const_iterator;

    private:
        struct Node
        {
            size_type hash = 0;
            Node* nextNode = nullptr;
            value_type value;
        };

        Node** _buckets;       // array of bucket heads
        size_type _bucketCount;
        size_type _size;
        float _maxLoadFactor;

        hasher _hash;
        key_equal _equal;
        allocator_type& _allocator;

        // ------------------------------------------------------------
        // Constructors / Destructor / Assignment
        // ------------------------------------------------------------
        explicit HashMap(
            TAllocator& allocator,
            size_type bucket_count = 16,
            const Hash& hash = Hash(),
            const KeyEqual& equal = KeyEqual());

        template<class InputIt>
        HashMap(
            TAllocator& allocator,
            InputIt first, InputIt last,
            size_type bucket_count = 16,
            const Hash& hash = Hash(),
            const KeyEqual& equal = KeyEqual());

        HashMap(std::initializer_list<value_type> init,
            size_type bucket_count = 16,
            const Hash& hash = Hash(),
            const KeyEqual& equal = KeyEqual());

        HashMap(const HashMap& other);
        HashMap(HashMap&& other) noexcept;
        ~HashMap();

        HashMap& operator=(const HashMap& other);
        HashMap& operator=(HashMap&& other) noexcept;
        HashMap& operator=(std::initializer_list<value_type> init);

        // ------------------------------------------------------------
        // Element Access
        // ------------------------------------------------------------
        mapped_type& at(const key_type& key);
        const mapped_type& at(const key_type& key) const;

        mapped_type& operator[](const key_type& key);
        mapped_type& operator[](key_type&& key);

        // ------------------------------------------------------------
        // Iterators
        // ------------------------------------------------------------
        iterator begin() noexcept;
        const_iterator begin() const noexcept;
        const_iterator cbegin() const noexcept;

        iterator end() noexcept;
        const_iterator end() const noexcept;
        const_iterator cend() const noexcept;

        // ------------------------------------------------------------
        // Capacity
        // ------------------------------------------------------------
        bool empty() const noexcept;
        size_type size() const noexcept;
        size_type max_size() const noexcept;

        // ------------------------------------------------------------
        // Modifiers
        // ------------------------------------------------------------
        void clear() noexcept;

        std::pair<iterator, bool> insert(const value_type& value);
        std::pair<iterator, bool> insert(value_type&& value);

        template<class InputIt>
        void insert(InputIt first, InputIt last);

        void insert(std::initializer_list<value_type> init);

        template<class... Args>
        std::pair<iterator, bool> emplace(Args&&... args);

        template<class... Args>
        iterator emplace_hint(const_iterator hint, Args&&... args);

        iterator erase(const_iterator pos);
        iterator erase(const_iterator first, const_iterator last);
        size_type erase(const key_type& key);

        void swap(HashMap& other) noexcept;

        // ------------------------------------------------------------
        // Lookup
        // ------------------------------------------------------------
        size_type count(const key_type& key) const;

        iterator find(const key_type& key);
        const_iterator find(const key_type& key) const;

        bool contains(const key_type& key) const;

        // ------------------------------------------------------------
        // Bucket Interface
        // ------------------------------------------------------------
        size_type bucket_count() const noexcept;
        size_type max_bucket_count() const noexcept;

        size_type bucket_size(size_type index) const;
        size_type bucket(const key_type& key) const;

        local_iterator begin(size_type bucket) noexcept;
        const_local_iterator begin(size_type bucket) const noexcept;

        local_iterator end(size_type bucket) noexcept;
        const_local_iterator end(size_type bucket) const noexcept;

        // ------------------------------------------------------------
        // Hash Policy
        // ------------------------------------------------------------
        float load_factor() const noexcept;
        float max_load_factor() const noexcept;
        void max_load_factor(float ml);

        void rehash(const size_type bucketsCount)
        {
            Node** newBuckets = _allocator.allocate(bucketsCount * sizeof(Node*));
            assert::inhouse(newBuckets, "Cannot allocate memory for buckets list");

            std::memset(newBuckets, 0, bucketsCount * sizeof(Node*));

            if(_buckets)
            {
                for(size_type i = 0; i < _bucketCount; ++i)
                {
                    Node* node = _buckets[i];
                    while(node)
                    {
                        Node* nextNode = node->nextNode;
                        const size_type bucketId = getBucketId(node->hash, bucketsCount);
                        node->nextNode = newBuckets[bucketId];
                        newBuckets[bucketId] = node;

                        node = nextNode;
                    }
                }

                _allocator.deallocate(_buckets, _bucketCount * sizeof(Node*));
            }

            _buckets = newBuckets;
            _bucketCount = bucketsCount;
        }

        void reserve(const size_type count)
        {
            const float maxLoadFactor = max_load_factor();
            assert::inhouse(maxLoadFactor > 0, "MAX load factor must be greater than zero");
            const size_type bucketsCount = bitwise::nextPowerOfTwo(static_cast<size_type>(std::ceil(static_cast<float>(count) / maxLoadFactor)));

            if(bucketsCount > _bucketCount)
            {
                rehash(bucketsCount);
            }
        }

        // ------------------------------------------------------------
        // Observers
        // ------------------------------------------------------------
        hasher hash_function() const;
        key_equal key_eq() const;

    private:
        // Helper functions you will implement
        void rehash_if_needed();
        void insert_unique_node(Node* node);

        static size_type getBucketId(const size_type hash, const size_type bucketsCount)
        {

            //get modulo
            // d - 1 works as a bit mask
            // and returns the k lowest
            // significant bits of n.
            return hash & (bucketsCount - 1);
        }

    };
}
