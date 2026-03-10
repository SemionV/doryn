# Hazard Pointer Memory Reclamation

This module implements a **Hazard Pointer–based memory reclamation system** for lock-free data structures.

Hazard pointers allow threads to safely access shared objects without locks while ensuring that objects removed from a data structure are **not reclaimed while another thread may still access them**.

The implementation is designed for:

- **Low-latency lock-free data structures**
- **Predictable memory usage**
- **Cache-friendly operation**
- **Minimal dynamic allocation**

The system is built around a **Domain** that manages hazard slots and retired objects.

---

# Overview

The implementation provides:

| Component | Purpose |
|----------|--------|
| `Domain` | Global hazard pointer manager |
| `Guard` | RAII object representing one hazard slot |
| `RetireList` | Per-thread list of retired nodes awaiting reclamation |
| `HazardSlot` | Atomic pointer published by threads to protect objects |
| `Janitor` | Callback used to reclaim retired objects |

The typical lifecycle of a node in a lock-free structure is:

```
allocate → publish → remove from structure → retire → scan → reclaim
```

Hazard pointers ensure reclamation only happens when **no thread still references the node**.

---

# Core Idea

When a thread accesses a shared pointer:

1. It **publishes the pointer in a hazard slot**.
2. Other threads see that the object is protected.
3. When a node is removed, it is **retired instead of immediately freed**.
4. During scanning, retired nodes that are **not protected by any hazard pointer are reclaimed**.

This prevents the classic use-after-free problem in lock-free algorithms.

---

# Domain

```
template <SizeType MaxThreads, SizeType SlotsPerThread, SizeType RetireCapacity>
class Domain
```

The `Domain` manages all hazard pointers and retired objects.

### Template Parameters

| Parameter | Description |
|----------|-------------|
| `MaxThreads` | Maximum number of participating threads |
| `SlotsPerThread` | Hazard pointers available per thread |
| `RetireCapacity` | Capacity of each thread's retire list |

### Internal Structure

```
Domain
 ├─ HazardSlot[MaxThreads * SlotsPerThread]
 └─ RetireList[MaxThreads]
```

- Hazard slots are **globally visible**.
- Retire lists are **thread-local**.

### Key Properties

- Fixed memory footprint
- No dynamic allocation
- Cache-line aligned hazard slots
- Deterministic scanning

---

# Guard

```
class Guard
```

`Guard` is an RAII wrapper representing a **single hazard slot**.

It ensures hazard pointers are automatically cleared when the guard goes out of scope.

### Example

```cpp
auto guard = domain.makeGuard(threadId);

Node* node = guard.getProtected(head);
```

When the guard is destroyed:

```
hazard_slot = nullptr
```

### Responsibilities

- Publish protected pointer
- Clear hazard pointer on destruction
- Provide safe pointer acquisition

### Important Rules

A `Guard`:

- **cannot be copied**
- **can be moved**
- **must remain alive while the pointer is used**

---

# Protecting Pointers

## protectRaw()

```
void protectRaw(void* p)
```

Publishes a pointer as a hazard.

Used when the pointer is already known to be stable.

Example:

```cpp
guard.protectRaw(node);
```

---

## getProtected()

```
T* getProtected(std::atomic<T*>& src)
```

Safely loads a pointer from an atomic source.

It ensures the pointer cannot change between load and hazard publication.

Algorithm:

```
loop:
    ptr = src.load()
    publish hazard(ptr)
until ptr == src.load()
```

This guarantees:

```
returned pointer is protected
```

Example:

```cpp
Node* node = guard.getProtected(head);
```

---

# Retiring Nodes

Nodes cannot be freed immediately after removal from a lock-free structure.

Instead they are **retired**.

```
void retire(ThreadId threadIndex, void* ptr, Janitor* janitor)
```

This pushes the node into the thread's retire list.

Example:

```cpp
domain.retire(threadId, node, janitor);
```

---

# Reclamation

Memory is reclaimed during **scan operations**.

```
void scan(ThreadId threadIndex)
```

Steps:

1. Snapshot all hazard pointers
2. Sort the snapshot
3. Check each retired node
4. If node is not in hazard set → reclaim

Pseudo logic:

```
for retired node:
    if node not in hazard set
        janitor.cleanup(node)
    else
        keep it
```

---

# Opportunistic Scanning

The implementation performs a scan automatically every **16 retire operations**.

```
if ((retired.count & 15) == 0)
    scan()
```

This reduces memory pressure without requiring explicit scans.

---

# Global Scanning

```
void scanAll()
```

Scans the retire lists of all threads.

Useful for:

- maintenance
- shutdown
- debugging

---

# Drain

```
void drain()
```

Runs a full scan across all threads.

This is automatically called in the `Domain` destructor.

Purpose:

```
cleanup all reclaimable nodes during shutdown
```

---

# Checking Hazard State

```
bool isHazard(void* ptr)
```

Returns true if the pointer is currently protected by any thread.

Primarily intended for:

- debugging
- testing
- verification

---

# Example Usage

## Lock-Free Stack Pop

```cpp
auto guard = domain.makeGuard(threadId);

Node* node = guard.getProtected(head);

if (node == nullptr)
    return nullptr;

Node* next = node->next;

if (head.compare_exchange_strong(node, next))
{
    guard.reset();
    domain.retire(threadId, node, janitor);
    return node;
}
```

Key points:

- pointer is protected before dereference
- node is retired after removal
- reclamation is deferred

---

# Memory Guarantees

Hazard pointers ensure:

```
A node will not be reclaimed while a hazard pointer references it.
```

More formally:

```
cleanup(ptr) happens only if ptr ∉ HazardSet
```

This eliminates:

- use-after-free
- ABA from reclaimed memory reuse

---

# Threading Model

The implementation assumes:

```
threadIndex ∈ [0, MaxThreads)
```

Each thread must use its **unique thread index**.

This avoids:

- dynamic thread registration
- global synchronization

---

# Performance Characteristics

## Time Complexity

| Operation | Complexity |
|----------|------------|
| Protect pointer | O(1) |
| Retire node | O(1) |
| Scan | O(H log H + R log H) |

Where:

```
H = number of hazard pointers
R = retired nodes
```

---

## Memory Usage

```
Hazard memory = MaxThreads * SlotsPerThread * sizeof(void*)
Retire memory = MaxThreads * RetireCapacity * sizeof(RetiredNode)
```

Total memory usage is **fully deterministic**.

---

# Design Goals

The implementation prioritizes:

- deterministic memory usage
- low overhead
- minimal synchronization
- cache friendliness
- simplicity

Key design choices include:

### Fixed-size arrays

Avoids dynamic allocation and fragmentation.

### Cache line alignment

Hazard slots are aligned to avoid false sharing.

### Per-thread retire lists

Reduces contention during retirement.

---

# Limitations

The system intentionally trades flexibility for performance.

### Static thread count

Threads must have a predefined index.

### Fixed hazard capacity

The number of hazard pointers is bounded.

### Fixed retire lists

Overflow triggers a scan and debug assertion.

---

# When to Use Hazard Pointers

Hazard pointers are ideal for:

- lock-free queues
- lock-free stacks
- concurrent hash tables
- RCU-like structures

They are less suitable when:

- thread count is unbounded
- memory reclamation latency must be extremely low
- dynamic thread registration is required

---

# Implementation Notes

Important implementation details:

- hazard pointers use `memory_order_release`
- hazard loads use `memory_order_acquire`
- scanning uses a sorted snapshot for fast lookup
- guards enforce automatic cleanup

These choices ensure correctness while minimizing synchronization cost.

---

# Summary

This hazard pointer system provides:

- safe memory reclamation for lock-free structures
- deterministic memory usage
- minimal runtime overhead
- simple integration via RAII guards

It is intended to be a **lightweight and high-performance reclamation strategy** for modern C++ concurrent data structures.

---