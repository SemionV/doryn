# Memory Reclamation: EBR and Hazard Pointers

Both schemes live in `components/data-structures/include/dory/data-structures/memory-reclamation/`. They are two static-polymorphism (CRTP) implementations of one domain interface, so a container is written once against "a domain" and doesn't know which scheme it gets.

## The generalized interface

There are three layers.

**1. Traits (compile-time configuration).** Each scheme has a traits struct that sets the domain's types and sizes:

| Trait member | EBR (`EpochBasedDomainTraits<MaxThreads, MaxRetired>`) | HP (`HazardPointersDomainTraits<MaxThreads, SlotsPerThread, MaxRetired>`) |
|---|---|---|
| `RetiredNodeType` | `EpochRetiredNode` (ptr, janitor, `retireEpoch`) | `RetiredNode` (ptr, janitor) |
| `RetireListType` | `RetireList<EpochRetiredNode, N>` | `RetireList<RetiredNode, N>` |
| `GuardType` | `Guard<EpochBasedDomain<…>>` | `Guard<HazardPointersDomain<…>>` |
| `pointerSlotsPerThread` | `numeric_limits<SizeType>::max()` (no real slots) | `SlotsPerThread` |

**2. `MemoryReclamationDomain<TImpl, TTraits>` (`domain.h`).** This is the CRTP base, built on `dory::Base` and its `implRef()`. It owns what both schemes share:

- **Per-thread retire lists:** `std::array<RetireListType, maxThreads> _retired`.
- **Public API:** `enter`, `leave`, `getPointerSlot`, `occupyPointerSlot`, `occupyAtomicPointerSlot`, `clearPointerSlot`, `tryAdvanceEpoch`, `retire`, `collect`. Each one forwards to `xxxImpl` on the derived class, so there are no virtual calls.
- **`makeGuard(threadId, pointerToken)`:** maps the token to a slot and returns a `GuardType`.
- **`collectAll()` / `drain()`:** guarded by an `atomic_flag`; they advance the epoch once and then collect every thread's retire list. The destructor calls `drain()`.

The interface is a superset of what each scheme needs. Each implementation turns the hooks it doesn't need into no-ops:

| Hook | EBR | HP |
|---|---|---|
| `enter` / `leave` | publish local epoch + `active` flag | no-op |
| `getPointerSlot` | returns 0 | `threadId * slotsPerThread + token` |
| `occupy…` / `clearPointerSlot` | no-op; atomic variant is a plain `load(acquire)` | store into the hazard slot (atomic variant uses the load/publish/re-validate loop) |
| `tryAdvanceEpoch` | advances the global epoch | no-op |
| `collect` | free nodes with `global - retireEpoch >= 2` | snapshot hazards, sort, free nodes not found by `binary_search` |

**3. `Guard<TDomain>` (`guard.h`).** This is a move-only RAII handle that is the same for both schemes:

- **Constructor:** calls `domain.enter()`. This is the EBR critical section.
- **`protectPointer` / `protectAtomicPointer`:** occupy the guard's slot. This is the HP publish.
- **`reset()`:** clears the slot.
- **Destructor:** calls `leave()` and then `reset()`.

So one guard expresses both "I'm in a critical section" (EBR) and "I hold this pointer" (HP).

**Deletion** is type-erased through `Janitor` (`janitor.h`), an interface with a virtual `cleanup(void*)`. `ObjectJanitor<T, TAllocator>` deallocates through the engine allocator. Retired nodes store `{void*, Janitor*}`, so one retire list can hold objects of mixed types. `RetireList::reclaimIf(pred)` compacts the list in place and calls the janitor on each node the predicate accepts.

## EBR (`epochBasedReclamation.h`)

- **State:** a cache-line-aligned `_globalEpoch` (starts at 1), plus a per-thread `ThreadEpochState {localEpoch, active}`, each padded to a cache line.
- **`enterImpl`:** reads the global epoch, stores it as `localEpoch`, sets `active = true`, then re-reads the global epoch and fixes `localEpoch` if it moved.
- **`tryAdvanceEpochImpl`:** scans the threads. If any active thread has `local <= current`, it gives up. Otherwise it CASes `current → current + 1`.
- **`retireImpl`:**
  - Tags the node with the current epoch.
  - If the list is full, it collects first, and asserts if the list is still full (reclamation stalled).
  - Every 8 retires it tries to advance the epoch and then collects.
- **`collectImpl`:** frees nodes whose age is at least 2. Age is computed as `global - retireEpoch` with unsigned arithmetic, so it stays correct if the epoch counter wraps.

## Hazard pointers (`hazardPointers.h`)

- **State:** a flat array of cache-line-aligned `HazardSlot`s, with `maxThreads * slotsPerThread` entries.
- **Protect:** the classic loop: load the source, publish it to the slot, and re-load until the value is stable.
- **`retireImpl`:** pushes the node. If the list is full it collects first. Every 16 retires it scans.
- **`collectImpl`:** snapshots the non-null hazards into a stack array, sorts them, and reclaims every retired node whose pointer is not in the snapshot. There's also a linear `isHazard()` helper.

## Consumer example

`HazardTreiberStack<T, TDomain, TAllocator>` (`containers/lockfree/stack.h`) is already generic over `TDomain`:

1. `pop` takes `_domain.makeGuard(tid, 0)`.
2. It reads the head with `guard.protectAtomicPointer(_head)`.
3. It CASes the head forward.
4. It calls `guard.reset()`, then `_domain.retire(tid, head, &_janitor)`.

With EBR the same code works: the guard opens and closes the epoch critical section, and protect becomes a plain acquire load. Only the name "Hazard" and the `hazardPointers.h` include tie it to HP. The tests only instantiate it with HP (`tests/unit-tests/src/data-structures/treiberStack.cpp:13`).

## Known issues

1. **No full fence between publishing and validating.**
   - EBR: `active.store(release)` followed by `_globalEpoch.load(acquire)` and the reader's later loads.
   - HP: the hazard `store(release)` followed by `src.load(acquire)` (`hazardPointers.h:75-77`).
   - Store→load reordering is allowed even on x86, so a collector can miss a reader that just published.
   - Both need `seq_cst` (or a `std::atomic_thread_fence(seq_cst)`) there, plus a matching fence on the collector side before it scans.
2. **`const_cast` in the epoch CAS** (`epochBasedReclamation.h:109`). A failed `compare_exchange_strong` writes to `current`, which is a `const` local. That is undefined behaviour; make `current` non-const.
3. **EBR never advances while any thread is active.** `local <= current` always holds for an active thread, because its local epoch can't exceed the global one. That's safe but stricter than standard EBR, which blocks only on `local != current`. Threads that are almost always inside a critical section can starve reclamation.
4. **`drain()` leaks in EBR.** It advances the epoch at most once, but `collect` requires age 2. Nodes retired in the last epoch are never freed when the domain is destroyed.
5. **The base destructor calls into the derived class after the derived part is destroyed.** `~MemoryReclamationDomain` → `drain()` → `implRef()` is undefined behaviour, even though trivially destructible atomics usually hide it. Moving `drain()` into the derived destructors fixes it.
6. **`collectAll()` races with owners' `retire()`.** It is public and walks other threads' non-atomic `RetireList`s. The `_collecting` flag only stops two `collectAll`s running at once, not a `collectAll` running alongside a thread's own retire. It's only safe once threads are quiescent, for example at shutdown.
