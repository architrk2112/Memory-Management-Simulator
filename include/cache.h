#ifndef CACHE_H
#define CACHE_H

#include <vector>
using namespace std;

struct CacheLine {
    bool valid;        
    int tag;           
    int fifo_order;    

    CacheLine() : valid(false), tag(-1), fifo_order(0) {}
};

// Cache class (can be used for L1 or L2)
class Cache {
private:
    int cache_size;        
    int block_size;        
    int associativity;     // 1 = direct mapped
    int num_sets;

    int access_counter;

    vector<vector<CacheLine>> sets;

    int hits;
    int misses;

public:
    Cache(int cacheSize, int blockSize, int assoc);

    bool access(int address); 

    int getHits() const;
    int getMisses() const;
};

#endif