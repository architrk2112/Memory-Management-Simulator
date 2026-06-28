#include "memory_manager.h"
#include <iostream>
#include <iomanip>
#include <climits>
using namespace std;

const int ALIGNMENT = 8;

MemoryManager::MemoryManager()
{
    total_memory = 0;
    allocator = AllocatorType::FIRST_FIT;
    next_id = 1;

    total_requests = 0;
    successful_allocations = 0;
    failed_allocations = 0;
    internal_fragmentation = 0;
}

void MemoryManager::init(int size)
{
    total_memory = size;
    blocks.clear();
    blocks.push_back(Block(0, size, true, -1));
    next_id = 1;

    total_requests = 0;
    successful_allocations = 0;
    failed_allocations = 0;
    internal_fragmentation = 0;
}

void MemoryManager::setAllocator(AllocatorType type)
{
    allocator = type;

    if (allocator == AllocatorType::BUDDY && total_memory > 0)
    {
        buddy.init(total_memory);
    }
}

list<Block>::iterator MemoryManager::findFirstFit(int size)
{
    for (auto it = blocks.begin(); it != blocks.end(); ++it)
        if (it->free && it->size >= size)
            return it;
    return blocks.end();
}

list<Block>::iterator MemoryManager::findBestFit(int size)
{
    auto best = blocks.end();
    int min_diff = INT_MAX;

    for (auto it = blocks.begin(); it != blocks.end(); ++it)
    {
        if (it->free && it->size >= size)
        {
            int diff = it->size - size;
            if (diff < min_diff)
            {
                min_diff = diff;
                best = it;
            }
        }
    }
    return best;
}

list<Block>::iterator MemoryManager::findWorstFit(int size)
{
    auto worst = blocks.end();
    int max_size = -1;

    for (auto it = blocks.begin(); it != blocks.end(); ++it)
    {
        if (it->free && it->size >= size && it->size > max_size)
        {
            max_size = it->size;
            worst = it;
        }
    }
    return worst;
}

int MemoryManager::allocate(int size, int &address)
{
    total_requests++;

    if (allocator == AllocatorType::BUDDY)
    {
        int id = buddy.allocate(size, address);
        if (id == -1)
            failed_allocations++;
        else
            successful_allocations++;
        return id;
    }

    int requested_size = size;
    int aligned_size = ((size + ALIGNMENT - 1) / ALIGNMENT) * ALIGNMENT;
    internal_fragmentation += (aligned_size - requested_size);
    size = aligned_size;

    if (size <= 0)
    {
        failed_allocations++;
        return -1;
    }

    list<Block>::iterator it;

    if (allocator == AllocatorType::FIRST_FIT)
        it = findFirstFit(size);
    else if (allocator == AllocatorType::BEST_FIT)
        it = findBestFit(size);
    else
        it = findWorstFit(size);

    if (it == blocks.end())
    {
        failed_allocations++;
        return -1;
    }

    int id = next_id++;

    if (it->size > size)
    {
        blocks.insert(next(it),
                      Block(it->start + size,
                            it->size - size,
                            true,
                            -1));
    }

    it->size = size;
    it->free = false;
    it->id = id;

    address = it->start;
    successful_allocations++;
    return id;
}

bool MemoryManager::freeBlock(int id)
{
    if (allocator == AllocatorType::BUDDY)
    {
        return buddy.freeBlock(id);
    }

    for (auto &b : blocks)
    {
        if (!b.free && b.id == id)
        {
            b.free = true;
            b.id = -1;
            coalesce();
            return true;
        }
    }
    return false;
}

void MemoryManager::coalesce()
{
    for (auto it = blocks.begin(); it != blocks.end();)
    {
        auto next_it = next(it);
        if (next_it != blocks.end() && it->free && next_it->free)
        {
            it->size += next_it->size;
            blocks.erase(next_it);
        }
        else
        {
            ++it;
        }
    }
}

void MemoryManager::dumpMemory() const
{
    if (allocator == AllocatorType::BUDDY)
    {
        buddy.dumpMemory();
        return;
    }

    cout << "\nMemory Layout:\n";
    for (const auto &b : blocks)
    {
        cout << "[0x" << hex << setw(4) << setfill('0') << b.start
             << " - 0x" << setw(4) << (b.start + b.size - 1) << "] ";

        if (b.free)
            cout << "FREE\n";
        else
            cout << "USED (id=" << dec << b.id << ")\n";
    }

    cout << dec;
}

void MemoryManager::printStats() const
{
    if (allocator == AllocatorType::BUDDY)
    {
        int used = buddy.getUsedMemory();
        int free_mem = buddy.getFreeMemory();

        double util = (double)used / total_memory * 100.0;

        cout << "\nStatistics:\n";
        cout << "Total memory: " << total_memory << "\n";
        cout << "Used memory : " << used << "\n";
        cout << "Free memory : " << free_mem << "\n";
        cout << fixed << setprecision(2);
        cout << "Utilization : " << util << "%\n";

        if (total_requests > 0)
        {
            double success_rate =
                (double)successful_allocations / total_requests * 100.0;
            double failure_rate =
                (double)failed_allocations / total_requests * 100.0;

            cout << "Allocation success rate: "
                 << success_rate << "%\n";
            cout << "Allocation failure rate: "
                 << failure_rate << "%\n";
        }
        return;
    }

    int used = 0;
    int free_mem = 0;
    int largest_free = 0;

    for (const auto &b : blocks)
    {
        if (b.free)
        {
            free_mem += b.size;
            largest_free = max(largest_free, b.size);
        }
        else
        {
            used += b.size;
        }
    }

    double utilization = (double)used / total_memory * 100.0;
    double external_frag =
        free_mem == 0 ? 0.0 : (double)(free_mem - largest_free) / free_mem * 100.0;

    cout << "\nStatistics:\n";
    cout << "Total memory: " << total_memory << "\n";
    cout << "Used memory : " << used << "\n";
    cout << "Free memory : " << free_mem << "\n";
    cout << fixed << setprecision(2) << "Utilization : " << utilization << "%\n";
    cout << "External fragmentation: " << external_frag << "%\n";
    cout << "Internal fragmentation: " << internal_fragmentation << " bytes\n";

    if (total_requests > 0)
    {
        double success_rate =
            (double)successful_allocations / total_requests * 100.0;
        double failure_rate =
            (double)failed_allocations / total_requests * 100.0;

        cout << "Allocation success rate: "
             << success_rate << "%\n";
        cout << "Allocation failure rate: "
             << failure_rate << "%\n";
    }
}