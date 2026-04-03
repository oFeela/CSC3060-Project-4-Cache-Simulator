#include "memory_hierarchy.h"
#include "prefetcher.h"
#include "repl_policy.h"
#include <cmath>
#include <iomanip>
#include <iostream>

using namespace std;

MainMemory::MainMemory(int lat) : latency(lat) {}

int MainMemory::access(uint64_t addr, char type, uint64_t cycle) {
    (void)addr;
    (void)type;
    (void)cycle;
    access_count++;
    return latency;
}

void MainMemory::printStats() {
    cout << "  [Main Memory] Total Accesses: " << access_count << endl;
}

CacheLevel::CacheLevel(string name, CacheConfig cfg, MemoryObject* next)
    : level_name(name), config(cfg), next_level(next) {
    policy = createReplacementPolicy(config.policy_name);
    prefetcher = createPrefetcher(config.prefetcher, config.block_size);

    uint64_t total_bytes = (uint64_t)config.size_kb * 1024;
    num_sets = total_bytes / (config.block_size * config.associativity);

    offset_bits = log2(config.block_size);
    index_bits = log2(num_sets);

    sets.resize(num_sets, vector<CacheLine>(config.associativity));

    cout << "Constructed " << level_name << ": "
         << config.size_kb << "KB, " << config.associativity << "-way, "
         << config.latency << "cyc, "
         << "[" << config.policy_name << " + " << prefetcher->getName() << "]" << endl;
}

CacheLevel::~CacheLevel() {
    delete policy;
    delete prefetcher;
}

uint64_t CacheLevel::get_index(uint64_t addr) {
    addr >>= offset_bits;
    uint64_t mask = (1ULL << index_bits) - 1;
    return (addr & mask);
}

uint64_t CacheLevel::get_tag(uint64_t addr) {
    addr >>= (index_bits + offset_bits);
    return addr;
}

/**
 * Reconstructs the 64-bits address from a given tag and set index.
 */
uint64_t CacheLevel::reconstruct_addr(uint64_t tag, uint64_t index) {
    // TODO: Task 1 / Task 2
    // Rebuild a block-aligned address from a tag and set index.
    // This helper is useful when writing back an evicted dirty line.

    // index bits cnt from log2(#of sets)
    // byte offset bits cnt from log2(size of cache line)
    // tag = 64 - index - byte

    // tag | index | byte
    // start from 00....00
    // append tag, index, byte
    // append by shifting appropriately and OR'ing
    // 0000, append (0)111 at the MSB --> shift by (4 - 3)
    // shift by 64 - tag size for tag
    // 0000, append (000)1 AFTER the tag bits, so it means you only have 64 - tag_size bits left
    // same logic
    // IT WORKS assuming the high bits for tag/index are all 0's (which should be the case from get_index, get_tag) 
    // unless the given tag, index are not from those helpers

    // 1st ver:
    // static const uint32_t bits_cnt = 64;
    // uint32_t tag_bits = bits_cnt - (this->offset_bits + this->index_bits);
    // uint32_t tag_shift = bits_cnt - tag_bits;
    // uint32_t index_shift = tag_shift - index_bits;
    
    // uint64_t ans = 0;
    // ans |= tag << tag_shift;
    // ans |= index << index_shift;

    // return ans;
    
    // 2nd ver:
    // uint64_t ans = 0;
    // ans |= tag << this->offset_bits + this->index_bits;
    // ans |= index << this->offset_bits;

    // return ans;

    return (tag << (this->offset_bits + this->index_bits)) | (index << this->offset_bits);

    // (void)tag;
    // (void)index;
    // return 0;
}

/**
 * @param index is the set index
 */
void CacheLevel::write_back_victim(const CacheLine& line, uint64_t index, uint64_t cycle) {
    // // TODO: Task 1 / Task 2
    // Suggested steps:
    // 1. If the victim is not dirty, return immediately.
    // 2. If there is no next level, return immediately.
    // 3. Increment the write-back counter.
    // 4. Reconstruct the evicted block address from tag + index.
    // 5. Send a write access to the next level.
    // Move dirty write-back logic into this helper.
    if (!line.dirty || this->next_level == nullptr) return;
    write_backs++;
    uint64_t addr = reconstruct_addr(line.tag, index);
    this->next_level->access(addr, 'w', cycle); // write
    // TODO does the cycle need to be added with latency? i dont have latency
}

int CacheLevel::access(uint64_t addr, char type, uint64_t cycle) {
    // // TODO: Task 1
    // 1. Derive the address fields for the current cache geometry:
    //    - block offset bits
    //    - set index bits
    //    - tag bits
    int lat = config.latency;
    uint64_t index = get_index(addr);
    uint64_t tag = get_tag(addr);

    // 2. Use the address to compute index/tag and select the set.
    auto& current_set = sets[index]; // the current row

    // 3. Search all ways for a valid tag match.
    for (size_t i = 0; i < current_set.size(); i++){
        auto& line = current_set[i];
        if (line.valid && line.tag == tag){
            // 4. On hit:
            //    - increment hits
            //    - call policy->onHit(...)
            //    - update dirty bit for writes
            //    - clear is_prefetched if a prefetched line is consumed
            hits++;
            policy->onHit(current_set, i, cycle); // cycle = current_cycle
            line.dirty = (type == 'w');
            if (line.is_prefetched) // correct now, if it was a prefetch line, then unmark it
                line.is_prefetched = false; 

            // TODO call prefetcher here ON HIT??? idk, depends on prefetch design ig
            // for (uint64_t addr_to_prefetch : prefetcher->calculatePrefetch(addr, false)) {
            //     this->install_prefetch(addr_to_prefetch, cycle);
            // }
            return lat;
        }
    }
    // 5. On miss:
    //    - increment misses
    //    - find an invalid line or select a victim with policy->getVictim(...)
    //    - call write_back_victim(...) if the chosen victim is dirty
    //    - fetch the requested block from next_level and add that latency to lat
    //    - install the new cache line and call policy->onMiss(...)
    misses++;
    int victim_way_index = policy->getVictim(current_set);
    write_back_victim(current_set[victim_way_index], index, cycle); // TODO cycle is probably current cycle, needed by write_back_victim


    // fetch next level
    //* MEM access can't possibly miss, so we just consider L1 access L2, or L2 access MEM
    if (next_level != nullptr)
        lat += this->next_level->access(addr, type, cycle);

    //* pretend to install new cache line
    current_set[victim_way_index].valid = true;
    current_set[victim_way_index].dirty = (type == 'w');
    current_set[victim_way_index].is_prefetched = false; // TODO correct? IDK TBH
    current_set[victim_way_index].tag = tag;

    policy->onMiss(current_set, victim_way_index, cycle);

    // 6. Your code should work correctly even if cache size, associativity,
    //    number of sets, or cache line size changes.
    // TODO Task 3: 
    // 7. after demand access logic works, call the prefetcher here and
    //    install returned blocks through install_prefetch(...).
    for (uint64_t addr_to_prefetch : prefetcher->calculatePrefetch(addr, true)) {
        this->install_prefetch(addr_to_prefetch, cycle);
    }

    return lat; // memory will have its own latency unchanged, because no addition
}

void CacheLevel::install_prefetch(uint64_t addr, uint64_t cycle) {
    uint64_t tag = get_tag(addr);
    uint64_t index = get_index(addr);

    auto& target_set = this->sets[index];

    // search if alr exists + valid
    for (size_t i = 0; i < target_set.size(); i++) {
        auto& line = target_set[i];
        if (line.valid && line.tag == tag) {
            return;
        }
    }

    // prepare to prefetch
    int victim_index = this->policy->getVictim(target_set);
    // in case we evict a dirty
    // need to write back to the next cache levels
    write_back_victim(target_set[victim_index], index, cycle);

    if (this->next_level != nullptr) {
        this->next_level->access(addr, 'r', cycle); // read from next level
        // no data writes needed on the current cache, cuz its just a simulation (bruh)
    }

    // install a new line on the current cache level
    target_set[victim_index].valid = true;
    target_set[victim_index].dirty = false;
    target_set[victim_index].is_prefetched = true;
    target_set[victim_index].tag = tag;

    // so, this will install a prefetched line in the current cache
    // which will be found by :access() later on yayyy

    // TODO: Task 3
    // Implement a prefetch fill path similar to the miss path in access(), but
    // treat prefetched lines as clean and mark is_prefetched = true.
    // If you evict a dirty victim during prefetch installation, reuse
    // write_back_victim(...) instead of duplicating that logic.
    // (void)addr;
    // (void)cycle;
}

void CacheLevel::printStats() {
    uint64_t total = hits + misses;
    cout << "  [" << level_name << "] "
         << "Hit Rate: " << fixed << setprecision(2) << (total ? (double)hits / total * 100.0 : 0) << "% "
         << "(Access: " << total << ", Miss: " << misses << ", WB: " << write_backs << ")" << endl;
    cout << "      Prefetches Issued: " << prefetch_issued << endl;
}
