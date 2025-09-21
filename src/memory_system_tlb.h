#include <iostream>
#include "OrderedDict.h"

using std::cout;
using std::endl;

class TLB
{
private:
    int size;
    OrderedDict<int, int> cache;
    int hits;
    int misses;

public:
    TLB(int tlb_size)
    {
        this->size = tlb_size;
        this->hits = 0;
        this->misses = 0;
    }

    int lookup(int virtual_page_number)
    {
        if (cache.contains(virtual_page_number))
        {
            hits++;
            cache.move_to_end(virtual_page_number);
            return cache[virtual_page_number];
        }
        else
        {
            misses++;
            return -1; // Indicate a miss
        }
    }

    void insert(int virtual_page_number, int physical_frame_number)
    {
        if (cache.contains(virtual_page_number))
        {
            cache.erase(virtual_page_number);
        }
        else if (cache.size() >= size)
        {
            // Evict the least recently used item
            auto lru_key = cache.get_order().front();
            cache.erase(lru_key);
        }
        cache.insert(virtual_page_number, physical_frame_number);
    }

    int hit_rate()
    {
        int total = hits + misses;
        return total == 0 ? 0 : (hits * 100) / total;
    }
};