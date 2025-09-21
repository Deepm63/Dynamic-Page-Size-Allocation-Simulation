#include <iostream>
#include <vector>
#include <string>
#include <utility>
#include <functional>
#include <iomanip>

// User-provided header files
// #include "policy_engine.h"
#include "memory_system_mmu.h"

// Use standard namespace for cleaner code
using std::cout;
using std::endl;
using std::string;
using std::vector;
using std::pair;
using std::function;

// --- Workloads ---

/**
 * @brief Simulates a database workload with one large memory allocation.
 * @return A vector containing a single allocation request (virtual address, size).
 */
vector<pair<int, int>> database_workload() {
    // One large allocation: 512 MB
    return {
        {0x10000000, 512 * 1024 * 1024}
    };
}

/**
 * @brief Simulates a web server workload with many small memory allocations.
 * @return A vector containing many small, consecutive allocation requests.
 */
vector<pair<int, int>> web_server_workload() {
    vector<pair<int, int>> requests;
    int base_va = 0x20000000;
    // 20,000 requests of 10 KB each
    for (int i = 0; i < 20000; ++i) {
        requests.push_back({base_va + (i * 12 * 1024), 10 * 1024});
    }
    return requests;
}

// --- Simulation Runner ---

/**
 * @brief Runs a memory simulation for a given policy and workload.
 * @param policy_mode The page size policy ("small", "large", or "dynamic").
 * @param workload_func A function that returns the workload requests.
 * @param workload_name The name of the workload for display purposes.
 */
void run_simulation(const string& policy_mode, const function<vector<pair<int, int>>()>& workload_func, const string& workload_name) {
    cout << "--- Running Simulation: Mode='" << policy_mode << "', Workload='" << workload_name << "' ---" << endl;

    // 1. Setup
    PolicyEngine policy_engine(policy_mode);
    MMU mmu(policy_engine);
    vector<pair<int, int>> workload = workload_func();

    // 2. Allocation Phase
    try {
        for (const auto& req : workload) {
            mmu.allocate(req.first, req.second);
        }
    } catch (const std::runtime_error& e) {
        cout << "Error during allocation: " << e.what() << endl;
        return;
    }


    // 3. Access Phase (Simulate random accesses to allocated memory)
    int num_accesses = 100000;
    for (int i = 0; i < num_accesses; ++i) {
        // Pick a request to access based on the current index
        const auto& req = workload[i % workload.size()];
        int req_va = req.first;
        int req_size = req.second;

        // Access a pseudo-random address within that allocated block
        int access_va = req_va + (i % req_size);
        try {
            mmu.translate(access_va);
        } catch (const std::runtime_error& e) {
            // This might happen if an address is invalid, though the logic should prevent it.
            cout << "Error during translation: " << e.what() << " for VA " << access_va << endl;
        }
    }

    // 4. Report Metrics
    // Note: We use the new getter methods to access the MMU's internal state.
    cout << std::fixed << std::setprecision(2); // Set output to 2 decimal places
    cout << "  TLB Hit Rate: " << mmu.get_tlb_hit_rate() << ".00%" << endl;
    cout << "  Internal Fragmentation: " << static_cast<double>(mmu.get_internal_fragmentation()) / (1024.0 * 1024.0) << " MB" << endl;
    cout << "  Page Table Size (Entries): " << mmu.get_page_table_size() << endl;
    cout << string(50, '-') << endl;
}


int main() {
    // Define the workloads and their names
    vector<function<vector<pair<int, int>>()>> workloads = {database_workload, web_server_workload};
    vector<string> workload_names = {"database_workload", "web_server_workload"};

    // Define the policy modes to test
    vector<string> modes = {"small", "large", "dynamic"};

    // Iterate through each workload and run simulations for each policy mode
    for (size_t i = 0; i < workloads.size(); ++i) {
        for (const auto& mode : modes) {
            run_simulation(mode, workloads[i], workload_names[i]);
        }
    }

    return 0;
}