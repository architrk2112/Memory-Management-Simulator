#ifndef BLOCK_H
#define BLOCK_H

struct Block
{
    int start; 
    int size;  
    bool free; 
    int id;    

    Block(int s, int sz, bool is_free = true, int block_id = -1)
        : start(s), size(sz), free(is_free), id(block_id) {}
};

#endif