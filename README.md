# Memory-Management-Simulator
## Demo Video
**Video Link:**  
https://drive.google.com/file/d/1cpfZ9JPUt8ah43wiWEva7W9qs4WckGoV/view?usp=sharing


##  Build and Run Instructions

### Compile using GNU g++
```bash
g++ -std=gnu++17 -Wall -Wextra -Iinclude src/main.cpp src/allocator/memory_manager.cpp src/cache/cache.cpp src/buddy/buddy.cpp -o memsim
```

### Run the simulator
```bash
./memsim
```

## Project Directory Structure

```
MEMORY MANAGEMENT
│
├── include
│   ├── block.h
│   ├── buddy.h
│   ├── cache.h
│   └── memory_manager.h
│
├── src
│   ├── allocator
│   │   └── memory_manager.cpp
│   ├── buddy
│   │   └── buddy.cpp
│   ├── cache
│   │   └── cache.cpp
│   └── main.cpp
│
├── tests
│   └── test_cases.txt
│
└── Makefile
```
## 1. Introduction

This project implements a **memory management simulator** that models how an operating system
manages **physical memory** and **CPU caches**.

The simulator is **not a real operating system**, but a **user-space simulation** focusing on:
- Correctness of memory allocation algorithms
- Memory layout and fragmentation behavior
- Cache performance trade-offs

The goal is to understand **core OS memory management concepts** through a clean and modular design.

---

## 2. Project Features

The simulator implements the following **mandatory features**:

- Contiguous physical memory allocation
- Dynamic memory allocation strategies
  - First Fit
  - Best Fit
  - Worst Fit
  - Buddy Allocation
- Memory deallocation with coalescing
- Fragmentation and memory utilization statistics
- Multilevel CPU cache simulation
  - L1 Cache
  - L2 Cache
- FIFO replacement policy
- Cache hit/miss tracking and propagation

Optional features such as **Buddy allocation**, **L3 cache**, and **Virtual Memory** are intentionally
**not implemented**, as allowed by the project specification.

---

## 3. Assumptions and Memory Model

### 3.1 Physical Memory

- Physical memory is simulated as a **single contiguous block**
- Memory size is configurable at runtime
- Memory is represented using logical units (bytes)
- No real hardware memory is accessed

### 3.2 Memory Blocks

Physical memory is divided into variable-sized **blocks**.

Each block contains:
- Starting address
- Block size
- Allocation status (free or allocated)
- Allocation ID (used for deallocation)

All blocks are stored in a **linked list**, preserving their physical order in memory.

---

## 4. Memory Allocation Strategies

The simulator supports three allocation strategies:

### 4.1 First Fit

- Allocates the **first free block** that is large enough
- Fast and simple
- May lead to fragmentation near the beginning of memory

### 4.2 Best Fit

- Allocates the **smallest free block** that satisfies the request
- Reduces wasted space
- Requires scanning all free blocks

### 4.3 Worst Fit

- Allocates the **largest available free block**
- Leaves medium-sized blocks available
- Can reduce small unusable fragments

### 4.4 Buddy Allocation

- Memory size must be a power of two
- Allocation sizes are rounded up to the nearest power of two
- Memory is managed using free lists indexed by block size
- Larger blocks are recursively split during allocation
- Deallocation uses XOR-based buddy computation and recursive coalescing

---

## 5. Block Splitting and Deallocation

### 5.1 Block Splitting

If a free block is larger than the requested size:
- The block is split into:
  - One allocated block
  - One remaining free block

For buddy allocation:
- Blocks are split recursively into equal halves until the required size is reached.

### 5.2 Deallocation and Coalescing

When a block is freed:
- It is marked as free
- Adjacent free blocks are **merged (coalesced)**

Coalescing helps:
- Reduce external fragmentation
- Restore larger contiguous free regions

For buddy allocation:
- The buddy block is computed using XOR
- Free buddy blocks of the same size are merged recursively

---

## 6. Fragmentation and Statistics

The simulator computes and reports the following metrics:

### 6.1 Memory Utilization

Memory utilization is calculated as:
```bash
Utilization = (Used Memory / Total Memory) × 100
```

---

### 6.2 Internal Fragmentation

To model realistic allocator behavior, memory allocations are aligned to 8 bytes.

Internal fragmentation is defined as:
```bash
Internal Fragmentation = Allocated Size − Requested Size
```

This represents unused memory inside allocated blocks due to alignment.

---

### 6.3 External Fragmentation

External fragmentation is calculated as:
```bash
External Fragmentation = (Total Free Memory − Largest Free Block) / Total Free Memory × 100
```

These metrics help compare the effectiveness of different allocation strategies.

---

### 6.4 Allocation Success and Failure Rates

The simulator tracks:

- Total allocation requests
- Successful allocations
- Failed allocations

Rates are computed as:

``` bash
Success Rate = Successful Allocations / Total Requests × 100

Failure Rate = Failed Allocations / Total Requests × 100
```

---

## 7. Cache Simulation Design

### 7.1 Cache Levels

The simulator implements a **two-level cache hierarchy**:

- L1 Cache (mandatory)
- L2 Cache (mandatory)

L3 cache is optional and not implemented.

---

### 7.2 Cache Organization

Each cache level is configurable with:
- Cache size
- Block size
- Associativity
  - Direct-mapped
  - Set-associative

Cache lines contain:
- Valid bit
- Tag
- FIFO ordering information

---

### 7.3 Replacement Policy

- FIFO (First-In First-Out) replacement is implemented
- LRU and LFU policies are optional and excluded

---

### 7.4 Cache Access Flow
```bash
CPU → L1 Cache → L2 Cache → Main Memory
```

- On an L1 cache miss, L2 is accessed
- On an L2 cache miss, the access goes to main memory (simulated)

---

### 7.5 Cache Statistics and Miss Propagation

Each cache level tracks:
- Cache hits
- Cache misses
- Hit ratio

Hit ratio is calculated as:
```bash
Hit Ratio = Hits / (Hits + Misses)
```

The simulator also reports:
- Number of L1 misses forwarded to L2
- Number of L2 misses forwarded to main memory

This models miss penalty propagation through the cache hierarchy.

---

## 8. Command Line Interface

The simulator provides a CLI with the following commands:

- `init memory <size>`
- `set allocator <first_fit | best_fit | worst_fit | buddy>`
- `malloc <size>`
- `free <block_id>`
- `dump memory`
- `stats`
- `access <address>`
- `exit`

This interface allows interactive testing of memory allocation and cache behavior.

---

## 9. Modular Design

The project is organized into modular components:

- `block.h`  
  Defines the memory block structure

- `memory_manager.*`  
  Implements memory allocation, deallocation, and statistics

- `buddy.*`
  Implements the buddy memory allocator

- `cache.*`  
  Implements L1 and L2 cache simulation

- `main.cpp`  
  Provides the command-line interface and integrates all modules

This modular design improves readability, maintainability, and extensibility.

---

## 10. Limitations and Simplifications

The following features are intentionally excluded:

- Virtual memory and paging
- L3 cache
- Write-back or dirty bits in cache
- LRU / LFU replacement policies
- Real hardware interaction

These simplifications keep the simulator focused on **core OS memory management concepts**.

---

## 11. Conclusion

This project demonstrates:
- Dynamic memory allocation strategies
- Fragmentation behavior and coalescing
- Buddy allocation using power-of-two blocks
- Memory utilization analysis
- Cache hierarchy and FIFO replacement behavior

The simulator provides a clear, educational, and extensible model of operating system
memory management.

---
