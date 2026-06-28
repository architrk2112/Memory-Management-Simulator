#include "cache.h"
#include <climits>
using namespace std;

Cache::Cache(int cacheSize, int blockSize, int assoc)
{
    cache_size = cacheSize;
    block_size = blockSize;
    associativity = assoc;

    int num_blocks = cache_size / block_size;
    num_sets = num_blocks / associativity;

    sets.resize(num_sets, vector<CacheLine>(associativity));

    access_counter = 0;
    hits = 0;
    misses = 0;
}

bool Cache::access(int address)
{
    access_counter++;

    int block_addr = address / block_size;
    int set_index = block_addr % num_sets;
    int tag = block_addr / num_sets;

    for (auto &line : sets[set_index])
    {
        if (line.valid && line.tag == tag)
        {
            hits++;
            return true;
        }
    }

    misses++;

    for (auto &line : sets[set_index])
    {
        if (!line.valid)
        {
            line.valid = true;
            line.tag = tag;
            line.fifo_order = access_counter;
            return false;
        }
    }

    int victim = 0;
    int oldest = INT_MAX;

    for (int i = 0; i < associativity; i++)
    {
        if (sets[set_index][i].fifo_order < oldest)
        {
            oldest = sets[set_index][i].fifo_order;
            victim = i;
        }
    }

    sets[set_index][victim].tag = tag;
    sets[set_index][victim].fifo_order = access_counter;

    return false;
}

int Cache::getHits() const
{
    return hits;
}

int Cache::getMisses() const
{
    return misses;
}