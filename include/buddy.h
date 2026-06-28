#ifndef BUDDY_H
#define BUDDY_H

#include <vector>
#include <list>
#include <unordered_map>
using namespace std;

class BuddyAllocator {
private:
    int total_memory;          
    int min_block_size;        
    int max_order;             // log2(total_memory / min_block_size)

    vector<list<int>> free_lists;

    unordered_map<int, pair<int, int>> allocations;

    int next_id;             

    int getOrder(int size) const;          
    int getBlockSize(int order) const;     
    int getBuddy(int address, int size) const; 

public:
    BuddyAllocator();

    void init(int memory_size);
    int allocate(int size, int &address);
    bool freeBlock(int id);
    void dumpMemory() const;

    int getUsedMemory() const;
    int getFreeMemory() const;
};

#endif
