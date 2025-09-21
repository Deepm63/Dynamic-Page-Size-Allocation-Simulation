#include <algorithm>
#include "memory_system_tlb.h"
#include "policy_engine.h"
#include "constants.h"

using std::distance;
using std::find;
using std::pair;
using std::runtime_error;

/**
 * @brief The Memory Management Unit orchestrates address translation and allocation.
 */
class MMU
{
private:
    TLB tlb;
    unordered_map<int, pair<int, int>> page_table; // Maps virtual page number to (physical frame number, page size)
    PolicyEngine policy_engine;
    // New: Simulate physical memory frames using a list as a free-list tracker
    // False means the frame is free, True means it's allocated.
    vector<bool> physical_frames;
    int internal_fragmentation;

public:
    MMU(PolicyEngine pe) : tlb(TLB_SIZE), policy_engine(pe), internal_fragmentation(0)
    {
        physical_frames.resize(PHYSICAL_MEMORY_SIZE / SMALL_PAGE_SIZE, false);
    }

    /**
     * @brief Finds and allocates a block of physical frames from the simulated free list.
     *  For num_frames > 1 (huge pages), it finds a contiguous block.
     *  For num_frames = 1 (small pages), it finds any single free frame.
     *
     * @param num_frames The number of contiguous frames to allocate
     * @return The starting index of the allocated frames, or -1 if allocation fails
     */
    int find_and_allocate_physical_frames(int num_frames)
    {
        // if (num_frames == 1)
        // {
        //     // Find the first available frame for a small page
        //     try
        //     {
        //         // Find the index of the first 'False' (free) frame
        //         auto it = find(physical_frames.begin(), physical_frames.end(), false);
        //         int frame_index = distance(physical_frames.begin(), it);
        //         physical_frames[frame_index] = true;
        //         return frame_index;
        //     }
        //     catch (...)
        //     {
        //         // If we couldn't find a free frame, throw an error
        //         throw runtime_error("Out of physical memory");
        //         return -1;
        //     }
        // }

        if (num_frames == 1)
        {
            // Find the first available frame for a small page.
            auto it = find(physical_frames.begin(), physical_frames.end(), false);

            // Correctly check if a free frame was found.
            if (it == physical_frames.end())
            {
                // No free frame found, so memory is full.
                return -1;
            }
            else
            {
                // A free frame was found. Mark it as allocated and return its index.
                *it = true; // Set the found element to true
                return distance(physical_frames.begin(), it);
            }
        }
        else
        {
            // Find a contiguous block of 'num_frames' for a huge page
            int consecutive_free_count = 0;
            int start_index = -1;
            for (size_t i = 0; i < physical_frames.size(); i++)
            {
                if (!physical_frames[i])
                {
                    if (consecutive_free_count == 0)
                    {
                        start_index = i;
                    }
                    consecutive_free_count++;
                }
                else
                {
                    consecutive_free_count = 0;
                    start_index = -1;
                }

                if (consecutive_free_count == num_frames)
                {
                    // Found a suitable contiguous block, now allocate it
                    for (int j = start_index; j < start_index + num_frames; j++)
                    {
                        physical_frames[j] = true;
                    }
                    return start_index;
                }
            }
            return -1; // Not enough contiguous frames available
        }
    }

    void allocate(int virtual_address, int request_size)
    {
        int page_size = policy_engine.decide_page_size(request_size);
        // int num_pages_needed = (request_size + page_size - 1) / page_size;

        // Correctly calculate the number of pages needed by considering the start and end addresses.
        int first_vpn = virtual_address / page_size;
        int last_vpn = (virtual_address + request_size - 1) / page_size;
        int num_pages_needed = last_vpn - first_vpn + 1;
        int allocated_memory = num_pages_needed * page_size;
        internal_fragmentation += (allocated_memory - request_size);

        for (int i = 0; i < num_pages_needed; i++)
        {
            // Each virtual page in a single allocation request is contiguous
            int virtual_page_number = (virtual_address / page_size) + i;

            if (page_table.find(virtual_page_number) == page_table.end())
            {
                // The physical frames for each virtual page are found independently
                // and are likely not contiguous with the frames for the previous virtual page.
                int number_frames_per_page = page_size / SMALL_PAGE_SIZE;
                int physical_frame_number = find_and_allocate_physical_frames(number_frames_per_page);
                if (physical_frame_number == -1)
                {
                    throw runtime_error("Out of physical memory");
                    return;
                }
                page_table[virtual_page_number] = {physical_frame_number, page_size};
            }
        }
    }

    void translate(int virtual_address)
    {
        int va_page_num = -1;
        int large_vpn = virtual_address / LARGE_PAGE_SIZE;
        if (page_table.find(large_vpn) != page_table.end() && page_table[large_vpn].second == LARGE_PAGE_SIZE)
        {
            va_page_num = large_vpn;
        }
        else
        {
            int small_vpn = virtual_address / SMALL_PAGE_SIZE;
            if (page_table.find(small_vpn) != page_table.end() && page_table[small_vpn].second == SMALL_PAGE_SIZE)
            {
                va_page_num = small_vpn;
            }
            else
            {
                throw runtime_error("Invalid virtual address");
                return;
            }
        }

        int physical_frame = tlb.lookup(va_page_num);

        if (physical_frame == -1)
        {
            physical_frame = page_table[va_page_num].first;
            tlb.insert(va_page_num, physical_frame);
        }
    }

    int get_tlb_hit_rate()
    {
        return tlb.hit_rate();
    }

    int get_internal_fragmentation() const
    {
        return internal_fragmentation;
    }

    size_t get_page_table_size() const
    {
        return page_table.size();
    }
};