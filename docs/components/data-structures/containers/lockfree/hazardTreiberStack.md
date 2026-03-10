# Hazard Treiber Stack

## Overview

The **Hazard Treiber Stack** is a **lock-free, thread-safe stack** based on the classic **Treiber stack algorithm**.  
It supports concurrent `push` and `pop` operations without locks and uses **hazard pointers** for safe memory reclamation.

This implementation guarantees:

- lock-free progress for stack operations
- safe memory reclamation via hazard pointers
- protection against use-after-free
- support for move-only payload types
- allocator-aware node creation

The structure is suitable for high-performance systems such as game engines, schedulers, and real-time pipelines.

---

# Algorithm

The Treiber stack is implemented as a **singly linked list** whose head pointer is updated using **atomic compare-and-swap (CAS)**.

Push and pop operations repeatedly attempt to update the head until the CAS succeeds.

### Push

```
push(value):
    node = new Node(value)

    do:
        oldHead = head
        node->next = oldHead
    while !CAS(head, oldHead, node)
```

### Pop

```
pop():
    do:
        head = protect(head)

        if head == null:
            return empty

        next = head->next
    while !CAS(head, head, next)

    retire(head)
    return value
```

The critical challenge is **safe memory reclamation**. A node cannot be destroyed immediately after removal because other threads may still read it.

This implementation solves the problem using **hazard pointers**.

---

# Hazard Pointer Integration

Hazard pointers allow threads to **publish which nodes they currently access**.

The `pop` algorithm works as follows:

1. Acquire a hazard pointer guard
2. Protect the current head pointer
3. Attempt a CAS removal
4. If CAS succeeds:
    - move the value out of the node
    - clear the hazard pointer
    - retire the node
5. The hazard pointer domain later safely reclaims retired nodes

This prevents **use-after-free** and eliminates the need for locks.

---

# Class Template

```
template <typename T, typename DomainT, typename TAllocator, LabelType AllocLabel = {}>
class HazardTreiberStack
```

### Template Parameters

| Parameter | Description |
|----------|-------------|
| `T` | Payload type stored in the stack |
| `DomainT` | Hazard pointer domain implementation |
| `TAllocator` | Allocator used for node allocation |
| `AllocLabel` | Optional allocation label |

---

# Public Interface

## Constructor

```
explicit HazardTreiberStack(DomainT& domain, TAllocator& allocator) noexcept;
```

Creates a stack bound to:

- a hazard pointer domain
- an allocator used for node allocation

The domain manages hazard pointers and retired nodes.

---

## push

```
template <typename U>
void push(U&& value);
```

Pushes a new element onto the stack.

### Behavior

1. Allocates a node using the allocator
2. Links the node to the current head
3. Updates the head pointer using CAS

### Complexity

```
O(1)
```

### Progress Guarantee

Lock-free.

At least one thread always makes progress.

### Example

```
stack.push(42);
stack.push(std::move(object));
```

---

## pop

```
std::optional<T> pop(ThreadId threadIndex);
```

Attempts to remove the top element.

### Behavior

1. Acquire a hazard pointer guard
2. Protect the head pointer
3. If the stack is empty return `std::nullopt`
4. Attempt CAS removal
5. On success:
    - move the payload out
    - reset the hazard pointer
    - retire the removed node
6. Return the value

### Complexity

```
O(1)
```

### Progress Guarantee

Lock-free.

### Return Value

| Condition | Result |
|---------|--------|
| Stack empty | `std::nullopt` |
| Element removed | `std::optional<T>` |

### Example

```
auto value = stack.pop(threadId);

if (value)
{
    process(*value);
}
```

---

## empty

```
bool empty() const noexcept;
```

Returns whether the stack appears empty.

### Behavior

Performs an atomic load of the head pointer.

### Note

Because the structure is concurrent, another thread may modify the stack immediately after this check.

---

# Memory Management

Nodes are allocated using the provided allocator:

```
_allocator.allocateObject<NodeType>(AllocLabel, value)
```

Removed nodes are **not deleted immediately**.  
Instead they are **retired** through the hazard pointer domain:

```
_domain.retire(threadIndex, node, &_janitor)
```

The domain determines when it is safe to reclaim them.

---

# Shutdown Semantics

Hazard pointer reclamation is **deferred**.

Nodes removed from the stack may remain in the domain's retired list until a reclamation scan occurs.

To ensure no retired nodes remain during shutdown, the hazard pointer domain should perform a **final drain or scan**.

Example:

```
domain.drain();
```

This guarantees that all retired nodes are reclaimed before destruction.

---

# Thread Safety

The stack is fully **thread-safe**.

Multiple threads may concurrently call:

- `push`
- `pop`
- `empty`

Each thread calling `pop` must provide a valid **thread identifier** used by the hazard pointer domain.

---

# Progress Guarantees

| Operation | Guarantee |
|----------|----------|
| push | lock-free |
| pop | lock-free |
| empty | wait-free |

Lock-free means the system as a whole always makes progress.

---

# ABA Safety

The classic Treiber stack is vulnerable to the **ABA problem** when nodes are reclaimed too early.

Hazard pointers prevent this by ensuring that nodes currently accessed by other threads cannot be reclaimed.

This removes ABA issues caused by premature memory reuse.

---

# Performance Characteristics

Advantages:

- minimal synchronization
- single atomic pointer update
- no locks or blocking
- high scalability under contention

Costs:

- hazard pointer bookkeeping
- deferred memory reclamation
- possible CAS retries

---

# Example Usage

```
HazardPointerDomain domain;
Allocator allocator;

HazardTreiberStack<int, HazardPointerDomain, Allocator> stack(domain, allocator);

stack.push(10);
stack.push(20);

auto value = stack.pop(threadId);

if (value)
{
    std::cout << *value << std::endl;
}
```

---

# When to Use

This stack is suitable for:

- task schedulers
- producer-consumer systems
- lock-free work queues
- runtime systems
- high-performance concurrent engines

---

# Limitations

1. The structure provides **LIFO semantics only**.
2. A **hazard pointer domain** must be provided.
3. Memory reclamation is **deferred** and depends on domain scanning.

---

# References

Treiber, R. K. (1986)  
Systems Programming: Coping with Parallelism

Michael, M. (2004)  
Hazard Pointers: Safe Memory Reclamation for Lock-Free Objects