#ifndef MEMORY_MANAGER_H
#define MEMORY_MANAGER_H

#include <list>
#include "block.h"
#include "buddy.h"
using namespace std;

enum class AllocatorType
{
    FIRST_FIT,
    BEST_FIT,
    WORST_FIT,
    BUDDY
};

class MemoryManager
{
private:
    int total_memory;        
    list<Block> blocks;      
    AllocatorType allocator;
    int next_id; 

    BuddyAllocator buddy;    

    int total_requests;
    int successful_allocations;
    int failed_allocations;
    int internal_fragmentation;

    list<Block>::iterator findFirstFit(int size);
    list<Block>::iterator findBestFit(int size);
    list<Block>::iterator findWorstFit(int size);

    void coalesce(); 

public:
    MemoryManager();

    void init(int size);              
    void setAllocator(AllocatorType); 

    int allocate(int size, int &address); 
    bool freeBlock(int id);

    void dumpMemory() const; 
    void printStats() const;

    int getTotalMemory() const; 
};

#endif