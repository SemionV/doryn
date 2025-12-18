#pragma once

#include <dory/macros/assert.h>
#include <dory/bitwise/numbers.h>
#include <functional>
#include <utility>
#include <initializer_list>
#include <iterator>
#include <cmath>
#include <cstring>

#include "string.h"

namespace dory::containers::hashMap
{
    template<
        typename THashMap,
        typename TNode,
        typename TValue,
        typename TSize,
        typename TDiff,
        bool IsConst
    >
    class HashMapIterator
    {
    private:
        using NodePtr = std::conditional_t<IsConst, const TNode*, TNode*>;
        using MapPtr  = std::conditional_t<IsConst, const THashMap*, THashMap*>;

        NodePtr _node = nullptr;
        MapPtr  _map = nullptr;
        TSize   _bucketIndex = 0;

    public:
        using iterator_category = std::forward_iterator_tag;
        using value_type        = TValue;
        using difference_type   = TDiff;
        using reference         = std::conditional_t<IsConst, const TValue&, TValue&>;
        using pointer           = std::conditional_t<IsConst, const TValue*, TValue*>;

        HashMapIterator() noexcept = default;

        HashMapIterator(MapPtr map, NodePtr node, TSize bucket)
            : _node(node), _map(map), _bucketIndex(bucket)
        {}

        // conversion: iterator → const_iterator
        template<bool B = IsConst, typename = std::enable_if_t<B>>
        explicit HashMapIterator(const HashMapIterator<THashMap, TNode, TValue, TSize, TDiff, false>& rhs)
            : _node(rhs._node), _map(rhs._map), _bucketIndex(rhs._bucketIndex)
        {}

        reference operator*() const noexcept { return _node->value; }
        pointer operator->() const noexcept { return &_node->value; }

        HashMapIterator& operator++()
        {
            if (_node && _node->nextNode)
            {
                _node = _node->nextNode;
            }
            else
            {
                const TSize bucketCount = _map->_bucketCount;
                TSize i = _bucketIndex + 1;

                while (i < bucketCount && !_map->_buckets[i])
                    ++i;

                if (i < bucketCount)
                {
                    _node = _map->_buckets[i];
                    _bucketIndex = i;
                }
                else
                {
                    _node = nullptr;
                    _bucketIndex = bucketCount;
                }
            }
            return *this;
        }

        HashMapIterator operator++(int)
        {
            HashMapIterator temp = *this;
            ++(*this);
            return temp;
        }

        friend bool operator==(const HashMapIterator& a, const HashMapIterator& b) noexcept
        {
            return a._node == b._node;
        }

        friend bool operator!=(const HashMapIterator& a, const HashMapIterator& b) noexcept
        {
            return a._node != b._node;
        }

        friend THashMap;
    };

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
    private:
        struct Node;

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

        using iterator        = HashMapIterator<HashMap, Node, value_type, size_type, difference_type, false>;
        using const_iterator  = HashMapIterator<HashMap, Node, value_type, size_type, difference_type, true>;

        using local_iterator        = HashMapIterator<HashMap, Node, value_type, size_type, difference_type, false>;
        using const_local_iterator  = HashMapIterator<HashMap, Node, value_type, size_type, difference_type, true>;

    private:
        struct Node
        {
            size_type hash = 0;
            Node* nextNode = nullptr;
            value_type value;
        };

        Node** _buckets;
        size_type _bucketCount = 0;
        size_type _size = 0;
        float _maxLoadFactor = 1.0;

        hasher _hash;
        key_equal _equal;
        allocator_type& _allocator;

    public:

        // ------------------------------------------------------------
        // Constructors / Destructor / Assignment
        // ------------------------------------------------------------
        explicit HashMap(
            TAllocator& allocator,
            const size_type bucketCount = 16,
            const Hash& hash = Hash(),
            const KeyEqual& equal = KeyEqual()):
        _hash(hash),
        _equal(equal),
        _allocator(allocator)
        {
            reserve(bucketCount);
        }

        /*template<class InputIt>
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
        HashMap(HashMap&& other) noexcept;*/

        ~HashMap()
        {
            if (_buckets)
            {
                for (size_type i = 0; i < _bucketCount; ++i)
                {
                    Node* node = _buckets[i];
                    while (node)
                    {
                        Node* nextNode = node->nextNode;

                        node->value.~value_type();
                        _allocator.deallocate(node, sizeof(Node));
                        node = nextNode;
                    }
                }

                _allocator.deallocate(_buckets, _bucketCount * sizeof(Node*));
            }
        }

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
        iterator begin() noexcept
        {
            if (_buckets)
            {
                for (size_type i = 0; i < _bucketCount; ++i)
                {
                    if(Node* node = _buckets[i])
                    {
                        return iterator{ this, node, i };
                    }
                }
            }

            return iterator{ this, nullptr, _bucketCount };
        }

        const_iterator begin() const noexcept
        {
            if (_buckets)
            {
                for (size_type i = 0; i < _bucketCount; ++i)
                {
                    if(Node* node = _buckets[i])
                    {
                        return const_iterator{ this, node, i };
                    }
                }
            }

            return const_iterator{ this, nullptr, _bucketCount };
        }

        const_iterator cbegin() const noexcept
        {
            return begin();
        }

        iterator end() noexcept
        {
            return iterator{ this, nullptr, _bucketCount };
        }

        const_iterator end() const noexcept
        {
            return const_iterator{ this, nullptr, _bucketCount };
        }

        const_iterator cend() const noexcept
        {
            return end();
        }

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

        std::pair<iterator, bool> insert(const value_type& value)
        {
            return insertValue(value);
        }

        std::pair<iterator, bool> insert(value_type&& value)
        {
            return insertValue(std::move(value));
        }

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
        size_type count(const key_type& key) const
        {
            return _size;
        }

        iterator find(const key_type& key)
        {
            return find<false>(key);
        }

        const_iterator find(const key_type& key) const
        {
            return find<true>(key);
        }

        bool contains(const key_type& key) const
        {
            const size_type hash = _hash(key);
            const size_type bucketId = getBucketId(hash, _bucketCount);

            assert::inhouse(bucketId < _bucketCount, "Invalid bucket index");

            Node* node = _buckets[bucketId];
            while(node)
            {
                if(node->hash == hash && _equal(node->value.first, key))
                    return true;

                node = node->nextNode;
            }

            return false;
        }

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

        [[nodiscard]] float max_load_factor() const noexcept
        {
            return _maxLoadFactor;
        }

        void max_load_factor(const float ml)
        {
            assert::inhouse(ml > 0.0f, "max_load_factor must be > 0");
            _maxLoadFactor = ml;
        }

        void rehash(const size_type bucketsCount)
        {
            Node** newBuckets = static_cast<Node**>(_allocator.allocate(bucketsCount * sizeof(Node*)));
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
        template<bool IsConst>
        auto find(const key_type& key) -> std::conditional_t<IsConst, const_iterator, iterator>
        {
            using TIterator = std::conditional_t<IsConst, const_iterator, iterator>;
            using NodePtr = std::conditional_t<IsConst, const Node*, Node*>;

            const size_type hash = _hash(key);
            const size_type bucketId = getBucketId(hash, _bucketCount);

            assert::inhouse(bucketId < _bucketCount, "Invalid bucket index");

            NodePtr node = _buckets[bucketId];
            while(node)
            {
                if(node->hash == hash && _equal(node->value.first, key))
                    break;

                node = node->nextNode;
            }

            if(node)
                return TIterator{ this, node, bucketId };

            if constexpr (IsConst)
                return cend();
            else
                return end();
        }

        template<typename U>
        std::pair<iterator, bool> insertValue(U&& value)
        {
            iterator it = find(value.first);
            if (it != end())
            {
                return { it, false }; // key exists → no insertion
            }

            // before allocating/hashing, ensure capacity
            if (_size + 1 > _bucketCount * _maxLoadFactor)
                rehash(bitwise::nextPowerOfTwo(_bucketCount + 1));

            // allocate node with allocator
            Node* node = allocateNode(std::forward<U>(value));

            // compute bucket
            size_t bucketId = getBucketId(node->hash, _bucketCount);

            // insert at head of bucket chain
            node->nextNode = _buckets[bucketId];
            _buckets[bucketId] = node;

            // update size
            ++_size;

            return { iterator(this, node, bucketId), true };
        }

        Node* allocateNode(value_type&& v)
        {
            Node* node = static_cast<Node*>(_allocator.allocate(sizeof(Node)));
            assert::inhouse(node, "HashMap Node was not allocated");

            new (&node->value) value_type(std::move(v));      // placement-new
            node->hash = _hash(node->value.first);
            node->nextNode = nullptr;

            return node;
        }

        Node* allocateNode(const value_type& v)
        {
            Node* node = static_cast<Node*>(_allocator.allocate(sizeof(Node)));
            assert::inhouse(node, "HashMap Node was not allocated");

            new (&node->value) value_type(v);                 // copy construct
            node->hash = _hash(node->value.first);
            node->nextNode = nullptr;

            return node;
        }

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
