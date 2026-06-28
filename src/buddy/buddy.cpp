#include <iostream>
#include <algorithm>
#include <cmath>
#include "buddy.h"
using namespace std;

BuddyAllocator::BuddyAllocator()
{
    total_memory = 0;
    min_block_size = 8;
    max_order = 0;
    next_id = 1;
}

int BuddyAllocator::getOrder(int size) const
{
    int order = 0;
    int block = min_block_size;

    while (block < size)
    {
        block <<= 1;
        order++;
    }
    return order;
}

int BuddyAllocator::getBlockSize(int order) const
{
    return min_block_size << order;
}

int BuddyAllocator::getBuddy(int address, int size) const
{
    return address ^ size;
}

void BuddyAllocator::init(int memory_size)
{
    if ((memory_size & (memory_size - 1)) != 0)
    {
        cout << "Error: Buddy allocator requires memory size to be a power of two\n";
        return;
    }

    total_memory = memory_size;
    allocations.clear();
    free_lists.clear();
    next_id = 1;

    max_order = 0;
    int size = memory_size;
    while (size > min_block_size)
    {
        size >>= 1;
        max_order++;
    }

    free_lists.resize(max_order + 1);

    free_lists[max_order].push_back(0);
}

int BuddyAllocator::allocate(int size, int &address)
{
    int order = getOrder(size);

    int current_order = order;
    while (current_order <= max_order && free_lists[current_order].empty())
    {
        current_order++;
    }

    if (current_order > max_order)
    {
        return -1;
    }

    int block_address = free_lists[current_order].front();
    free_lists[current_order].pop_front();

    while (current_order > order)
    {
        current_order--;

        int block_size = getBlockSize(current_order);
        int buddy_address = block_address + block_size;

        free_lists[current_order].push_back(buddy_address);
    }

    int id = next_id++;
    int final_size = getBlockSize(order);

    allocations[id] = {block_address, final_size};
    address = block_address;

    return id;
}

bool BuddyAllocator::freeBlock(int id)
{
    auto it = allocations.find(id);
    if (it == allocations.end())
    {
        return false;
    }

    int address = it->second.first;
    int size = it->second.second;

    allocations.erase(it);

    int order = getOrder(size);

    while (order < max_order)
    {
        int buddy_address = getBuddy(address, getBlockSize(order));

        auto &list = free_lists[order];
        auto buddy_it = list.begin();
        bool found = false;

        for (; buddy_it != list.end(); ++buddy_it)
        {
            if (*buddy_it == buddy_address)
            {
                found = true;
                break;
            }
        }

        if (!found)
            break;

        list.erase(buddy_it);

        address = min(address, buddy_address);
        order++;
    }

    free_lists[order].push_back(address);
    return true;
}

void BuddyAllocator::dumpMemory() const
{
    cout << "\nBuddy Memory State:\n";

    cout << "\nFree Lists:\n";
    for (int i = 0; i <= max_order; i++)
    {
        cout << "Order " << i
             << " (size " << getBlockSize(i) << "): ";

        if (free_lists[i].empty())
        {
            cout << "[]";
        }
        else
        {
            cout << "[ ";
            for (int addr : free_lists[i])
                cout << addr << " ";
            cout << "]";
        }
        cout << "\n";
    }

    cout << "\nAllocated Blocks:\n";
    if (allocations.empty())
    {
        cout << "None\n";
    }

    else
    {
        for (const auto &p : allocations)
        {
            cout << "ID=" << p.first
                 << " Address=" << p.second.first
                 << " Size=" << p.second.second
                 << "\n";
        }
    }
}

int BuddyAllocator::getUsedMemory() const
{
    int used = 0;
    for (const auto &p : allocations)
        used += p.second.second;
    return used;
}

int BuddyAllocator::getFreeMemory() const
{
    int free_mem = 0;
    for (int i = 0; i <= max_order; i++)
    {
        int block_size = getBlockSize(i);
        free_mem += free_lists[i].size() * block_size;
    }
    return free_mem;
}
