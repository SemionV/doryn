# TrackingList

`TrackingList` is a lightweight lock-free container designed for **concurrent tracking of objects** in systems where **elements are only appended during runtime and drained later in a quiescent state**.

It is implemented as a **Treiber-style lock-free prepend list**, optimized for extremely cheap concurrent `push()` operations.

Unlike a full concurrent stack, `TrackingList` **does not support concurrent pop operations**. Instead, the entire list is **drained in a single-threaded context** using `clear()`.

This design keeps the runtime path minimal and avoids the complexity of memory reclamation strategies such as hazard pointers or epoch-based reclamation.

---

# Primary Use Case

`TrackingList` is intended for **engine bookkeeping and tracking tasks**, such as:

- Tracking large allocated memory blocks
- Recording resources that must be released at shutdown
- Registering objects for deferred cleanup
- Tracking allocations for diagnostics or leak detection
- Recording engine state during runtime for later inspection

Typical pattern:

```
multiple threads -> push items concurrently
engine shutdown -> single-threaded clear()
```

Because no items are removed during runtime, the structure avoids reclamation complexity and remains extremely efficient.

---

# Properties

| Property | Description |
|--------|--------|
| Thread Safety | Multiple producers (`push`) are thread-safe |
| Lock-Free | `push()` is lock-free |
| Pop Support | No concurrent pop |
| Memory Reclamation | Deferred until `clear()` |
| Ordering | LIFO order during drain |
| Iteration | Not supported during runtime |

---

# Design

Internally the container maintains a single atomic head pointer:

```
atomic<Node*> head
```

Nodes are prepended using a standard **CAS loop**:

```
newNode->next = head
CAS(head, newNode)
```

This is the classic **Treiber stack push algorithm**, but with **no pop operation**.

When `clear()` is called, the entire list is detached atomically:

```
head.exchange(nullptr)
```

The caller then owns the entire list and can safely walk and destroy nodes.

---

# API

## Constructor

```
TrackingList(TAllocator& allocator)
```

The container uses the provided allocator for node allocation and deallocation.

---

## push

```
template<typename... TArgs>
void push(TArgs&&... args);
```

Constructs a new element and pushes it onto the tracking list.

### Thread Safety

Safe for **concurrent calls from multiple threads**.

### Complexity

O(1) lock-free operation.

---

## clear

```
template<typename F>
void clear(F&& fn);
```

Drains the entire list.

The callback `fn` is invoked for each element before its node is destroyed.

Example:

```
list.clear([](Resource& r) {
    r.release();
});
```

### Guarantees

- All elements currently in the list will be processed.
- The list becomes empty after the call.

### Order

Elements are visited in **LIFO order**.

---

# Threading Contract

`push()` may be called concurrently by multiple threads.

`clear()` **must only be called in a quiescent state**, meaning:

- No other thread is calling `push()`
- No thread retains references to nodes in the list

Violating this contract results in undefined behavior.

Typical usage:

```
runtime threads push elements
↓
engine stops worker threads
↓
TrackingList::clear() drains items
```

---

# Memory Management

Nodes are allocated using:

```
allocator.allocateObject<Node>()
```

and destroyed using:

```
allocator.deallocateObject<Node>()
```

The allocator must properly destroy the stored object `T`.

Memory reclamation occurs **only during `clear()` or destruction**.

---

# Performance Characteristics

Advantages:

- extremely cheap concurrent `push`
- minimal synchronization
- no runtime memory reclamation cost
- cache-friendly structure

Tradeoffs:

- no concurrent removal
- order of elements is LIFO
- requires external shutdown synchronization

---

# Example Usage

```
TrackingList<Resource, EngineAllocator, Label::Resources> resources(allocator);

// runtime threads
resources.push(Resource("texture"));
resources.push(Resource("mesh"));
resources.push(Resource("audio"));

// shutdown
resources.clear([](Resource& r) {
    r.destroy();
});
```

---

# When To Use

`TrackingList` is ideal when:

- elements are **only added during runtime**
- removal happens **only during shutdown or phase transition**
- runtime operations must be **very fast and contention-friendly**

---

# When Not To Use

Do not use `TrackingList` when:

- elements must be removed during runtime
- iteration during concurrent mutation is required
- FIFO ordering is needed
- fine-grained reclamation is required

For those use cases, a **full concurrent stack or queue** with proper memory reclamation should be used.

---

# Summary

`TrackingList` provides a **minimal, fast, and predictable structure for concurrent tracking tasks** where items are collected during runtime and drained later in a controlled environment.

It is especially suited for **engine-level bookkeeping and shutdown cleanup systems**.