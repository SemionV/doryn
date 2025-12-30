#pragma once

#include <dory/macros/assert.h>
#include <dory/bitwise/numbers.h>
#include <functional>
#include <utility>
#include <initializer_list>
#include <iterator>
#include <cmath>
#include <cstring>

namespace dory::data_structures::containers::hashMap
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

        template<class InputIt>
        HashMap(
            TAllocator& allocator,
            InputIt first, InputIt last,
            const size_type bucket_count = 16,
            const Hash& hash = Hash(),
            const KeyEqual& equal = KeyEqual())
            : _hash(hash)
            , _equal(equal)
            , _allocator(allocator)
        {
            _size = 0;
            _maxLoadFactor = 1.0f;   // or whatever your default is
            _buckets = nullptr;
            _bucketCount = 0;

            allocate_buckets(bucket_count);

            // Optional optimization: only for forward iterators and better
            if constexpr (std::forward_iterator<InputIt>)
            {
                const auto n = static_cast<size_type>(std::distance(first, last));
                reserve(n);
            }

            insert(first, last);
        }

        HashMap(TAllocator& allocator,
            std::initializer_list<value_type> init,
            const size_type bucket_count = 16,
            const Hash& hash = Hash(),
            const KeyEqual& equal = KeyEqual())
        requires (std::is_copy_constructible_v<key_type> && std::is_copy_constructible_v<mapped_type>)
        : _hash(hash), _equal(equal), _allocator(allocator)
        {
            _size = 0;
            _maxLoadFactor = 1.0f;
            _buckets = nullptr;
            _bucketCount = 0;

            allocate_buckets(bucket_count);

            reserve(init.size());
            insert(init.begin(), init.end());
        }

        HashMap(const HashMap& other)
        requires (std::is_copy_constructible_v<key_type> && std::is_copy_constructible_v<mapped_type>)
        : _maxLoadFactor(other._maxLoadFactor),
        _hash(other._hash),
        _equal(other._equal),
        _allocator(other._allocator)
        {
            _size = 0;
            _buckets = nullptr;
            _bucketCount = 0;

            allocate_buckets(other._bucketCount);
            reserve(other._size);

            for (const auto& kv : other)
                insert(kv); // copies
        }

        HashMap(HashMap&& other) noexcept:
        _buckets(other._buckets),
        _bucketCount(other._bucketCount),
        _size(other._size),
        _maxLoadFactor(other._maxLoadFactor),
        _hash(std::move(other._hash)),
        _equal(std::move(other._equal)),
        _allocator(std::move(other._allocator))
        {
            other._buckets = nullptr;
            other._bucketCount = 0;
            other._size = 0;
            other._maxLoadFactor = 1.0f; // optional reset
        }

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

        HashMap& operator=(const HashMap& other)
        requires (std::is_copy_constructible_v<key_type> && std::is_copy_constructible_v<mapped_type>)
        {
            if (this == &other) return *this;

            clear();               // keep bucket array, just remove nodes
            reserve(other.size()); // avoid rehash churn

            for (const auto& kv : other)
                insert(kv);        // must copy (kv is const)

            return *this;
        }

        HashMap& operator=(HashMap&& other) noexcept
        {
            if (this == &other) return *this;

            // Free current resources
            clear();
            if (_buckets)
                _allocator.deallocate(_buckets, _bucketCount * sizeof(Node*));

            // Steal other's internals
            _buckets        = other._buckets;
            _bucketCount    = other._bucketCount;
            _size           = other._size;
            _maxLoadFactor  = other._maxLoadFactor;
            _hash           = std::move(other._hash);
            _equal          = std::move(other._equal);
            _allocator      = std::move(other._allocator); // IMPORTANT if allocator owns the memory

            // Leave other valid
            other._buckets = nullptr;
            other._bucketCount = 0;
            other._size = 0;

            return *this;
        }

        HashMap& operator=(std::initializer_list<value_type> init)
        requires (std::is_copy_constructible_v<key_type> && std::is_copy_constructible_v<mapped_type>)
        {
            clear();
            reserve(_size + init.size()); // optional
            insert(init.begin(), init.end());
            return *this;
        }

        // ------------------------------------------------------------
        // Element Access
        // ------------------------------------------------------------
        mapped_type& at(const key_type& key) noexcept
        {
            auto it = find(key);
            assert::inhouse(it != end(), "HashMap::at: key not found");
            return it->second;
        }

        const mapped_type& at(const key_type& key) const noexcept
        {
            auto it = find(key);
            assert::inhouse(it != cend(), "HashMap::at: key not found");
            return it->second;
        }

        mapped_type& operator[](const key_type& key) noexcept
        {
            return get_or_insert_default_noexcept(key);
        }

        mapped_type& operator[](key_type&& key) noexcept
        {
            return get_or_insert_default_noexcept(std::move(key));
        }

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
        [[nodiscard]] bool empty() const noexcept
        {
            return _size == 0;
        }

        [[nodiscard]] size_type size() const noexcept
        {
            return _size;
        }

        [[nodiscard]] static size_type max_size() noexcept
        {
            return std::numeric_limits<size_type>::max() / sizeof(Node);
        }

        // ------------------------------------------------------------
        // Modifiers
        // ------------------------------------------------------------
        void clear() noexcept
        {
            if (!_buckets) {
                _size = 0;
                return;
            }

            for (size_type i = 0; i < _bucketCount; ++i)
            {
                Node* node = _buckets[i];
                while (node)
                {
                    Node* next = node->nextNode;
                    destroyNode(node);
                    node = next;
                }
                _buckets[i] = nullptr;
            }

            _size = 0;
        }

        std::pair<iterator, bool> insert(const value_type& value)
        {
            return insertValue(value);
        }

        std::pair<iterator, bool> insert(value_type&& value)
        {
            return insertValue(std::move(value));
        }

        template<class InputIt>
        void insert(InputIt first, InputIt last)
        {
            while(first != last)
            {
                insert(*first);
                ++first;
            }
        }

        void insert(std::initializer_list<value_type> init)
        {
            insert(init.begin(), init.end());
        }

        template<class... Args>
        std::pair<iterator, bool> emplace(Args&&... args)
        {
            value_type v(std::forward<Args>(args)...);
            return insert(std::move(v));
        }

        template<class... Args>
        iterator emplace_hint(const_iterator /*hint*/, Args&&... args)
        {
            return emplace(std::forward<Args>(args)...).first;
        }

        iterator erase(const_iterator pos)
        {
            assert::inhouse(pos._map == this, "Iterator does not belong to this HashMap");
            assert::inhouse(pos._node != nullptr, "Cannot erase end()");

            const size_type bucketId = pos._bucketIndex;
            Node* target = const_cast<Node*>(pos._node);

            // 1) Compute iterator to the next element (BEFORE erase)
            iterator nextIt;
            if (target->nextNode)
            {
                nextIt = iterator{ this, target->nextNode, bucketId };
            }
            else
            {
                size_type i = bucketId + 1;
                while (i < _bucketCount && !_buckets[i])
                    ++i;

                if (i < _bucketCount)
                    nextIt = iterator{ this, _buckets[i], i };
                else
                    nextIt = end();
            }

            // 2) Unlink from bucket chain
            Node*& head = _buckets[bucketId];
            if (head == target)
            {
                head = target->nextNode;
            }
            else
            {
                Node* prev = head;
                while (prev->nextNode != target)
                    prev = prev->nextNode;

                prev->nextNode = target->nextNode;
            }

            // 3) Destroy node
            destroyNode(target);
            --_size;

            // 4) Return next iterator
            return nextIt;
        }

        iterator erase(const_iterator first, const_iterator last)
        {
            while (first != last)
                first = erase(first);           // erase(pos) returns iterator, which converts to const_iterator

            // Now first == last; return iterator at that position.
            return iterator{ this, const_cast<Node*>(last._node), last._bucketIndex };
        }

        size_type erase(const key_type& key)
        {
            if (_bucketCount == 0 || !_buckets)
                return 0;

            const size_type hash = _hash(key);
            const size_type bucketId = getBucketId(hash, _bucketCount);

            Node* node = _buckets[bucketId];
            Node* prev = nullptr;

            while (node)
            {
                if (node->hash == hash && _equal(node->value.first, key))
                {
                    // unlink
                    if (prev)
                        prev->nextNode = node->nextNode;
                    else
                        _buckets[bucketId] = node->nextNode;

                    destroyNode(node);
                    --_size;
                    return 1;
                }

                prev = node;
                node = node->nextNode;
            }

            return 0;
        }

        void swap(HashMap& other) noexcept
        {
            using std::swap;

            swap(_buckets, other._buckets);
            swap(_bucketCount, other._bucketCount);
            swap(_size, other._size);
            swap(_maxLoadFactor, other._maxLoadFactor);

            swap(_hash, other._hash);
            swap(_equal, other._equal);

            // Important for byte-allocator correctness:
            swap(_allocator, other._allocator);
        }

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
        [[nodiscard]] float load_factor() const noexcept
        {
            return static_cast<float>(_size) / _bucketCount;
        }

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
        hasher hash_function() const
        {
            return _hash;
        }

        key_equal key_eq() const
        {
            return _equal;
        }

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

        template<class K>
        mapped_type& get_or_insert_default_noexcept(K&& key) noexcept
        {
            static_assert(std::is_default_constructible_v<mapped_type>, "HashMap::operator[] requires default-constructible mapped_type");

            const size_type h = _hash(key);
            size_type bucketId = getBucketId(h, _bucketCount);

            // find
            for (Node* n = _buckets[bucketId]; n; n = n->nextNode)
            {
                if (n->hash == h && _equal(n->value.first, key))
                    return n->value.second;
            }

            // grow if needed (your policy)
            if ((_size + 1) > static_cast<size_type>(_bucketCount * max_load_factor()))
                reserve(_size + 1); // or rehash_if_needed_for_insert()

            bucketId = getBucketId(h, _bucketCount);

            Node* node = static_cast<Node*>(_allocator.allocate(sizeof(Node)));
            assert::inhouse(node, "HashMap Node was not allocated");

#ifndef __cpp_exceptions
            static_assert(
                std::is_nothrow_constructible_v<value_type,
                    decltype(std::piecewise_construct),
                    std::tuple<K&&>,
                    std::tuple<>
                >,
                "HashMap::operator[] requires noexcept construction when exceptions are disabled."
            );
#endif

            new (&node->value) value_type(
                std::piecewise_construct,
                std::forward_as_tuple(std::forward<K>(key)),
                std::forward_as_tuple() // default-construct mapped_type
            );

            node->hash = h;
            node->nextNode = _buckets[bucketId];
            _buckets[bucketId] = node;
            ++_size;

            return node->value.second;
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

        void destroyNode(Node* node)
        {
            if (!node) return;

            node->value.~value_type();

#if defined(DEBUG) || defined(_DEBUG)
            node->hash = 0;
            node->nextNode = nullptr;
#endif

            _allocator.deallocate(node, sizeof(Node));
        }

        static size_type normalize_bucket_count(size_type n)
        {
            if (n < 1) n = 1;
            return bitwise::nextPowerOfTwo(n);
        }

        void allocate_buckets(size_type bucket_count)
        {
            _bucketCount = normalize_bucket_count(bucket_count);

            _buckets = static_cast<Node**>(_allocator.allocate(_bucketCount * sizeof(Node*)));
            assert::inhouse(_buckets, "Cannot allocate buckets");

            // zero-init bucket heads
            for (size_type i = 0; i < _bucketCount; ++i)
                _buckets[i] = nullptr;
        }

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
