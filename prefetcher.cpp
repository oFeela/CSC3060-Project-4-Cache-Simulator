#include "prefetcher.h"

// TODO i have no clue what 'miss' param is for

/**
 * @return vector of addresses to prefetch
 ** CacheLevel will install them to the cache
 * @param current_addr that triggered prefetch
 * @param miss only triggered on miss? IDK
 */
std::vector<uint64_t> NextLinePrefetcher::calculatePrefetch(uint64_t current_addr, bool miss) {
    (void)current_addr;
    (void)miss;
    std::vector<uint64_t> prefetches;

    // TODO: Task 3
    // 1. Align current_addr down to the current cache block.
    // 2. Prefetch the next sequential block.

    return prefetches;
}

/**
 * @return vector of addresses to prefetch
 ** CacheLevel will install them to the cache
 * @param current_addr that triggered prefetch
 * @param miss only triggered on miss? IDK
 */
std::vector<uint64_t> StridePrefetcher::calculatePrefetch(uint64_t current_addr, bool miss) {
    (void)current_addr;
    (void)miss;

    //* calculate address with no bottom bits
    uint64_t current_block = current_addr / block_size;

    // first time access
    if (!this->has_last_block) {
        has_last_block = true;
        last_block = current_block;
        return {};
    }

    int64_t current_stride = current_block - last_block;

    // second time access
    if (has_last_block && last_stride == 0){
        last_stride = current_stride;
        confidence = 1;
        last_block = current_block;
        return {};
    }

    // third and subsequent accesses 
    std::vector<uint64_t> blocks = {};
    if (last_stride == current_stride){
        confidence++;

        uint32_t threshold = 2;
        if (confidence >= threshold){
            //* prefetch (can change N)
            int N = 2;
            for (int i = 1; i <= N; i++){
                uint64_t new_block = current_block + (i*last_stride);
                blocks.push_back(new_block);
            }
        }
    }
    else {
        last_stride = current_stride;
        confidence--;
    }

    last_block = current_block; // update
    return blocks; // may be {}

    // TODO: Task 3
    // Suggested design:
    // 1. Track the previous accessed block.
    // 2. Compute the current stride in block units.
    // 3. If the same stride repeats enough times, prefetch the next block at that stride.
    // 4. Update last_block / last_stride / confidence.
}

Prefetcher* createPrefetcher(std::string name, uint32_t block_size) {
    if (name == "NextLine") return new NextLinePrefetcher(block_size);
    if (name == "Stride") return new StridePrefetcher(block_size);
    return new NoPrefetcher();
}
