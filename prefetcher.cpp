#include "prefetcher.h"

// TODO i have no clue what 'miss' param is for

/**
 * @return vector of addresses to prefetch
 ** CacheLevel will install them to the cache
 * @param current_addr that triggered prefetch
 * @param miss only triggered on miss? IDK
 */
std::vector<uint64_t> NextLinePrefetcher::calculatePrefetch(uint64_t current_addr, bool miss) {
    // (void)current_addr;
    // (void)miss;
    std::vector<uint64_t> prefetches;
    int64_t prefetch_count = 1; // how many next blocks to prefetch, default is 1, literally next line

    // R shifts by log2(block_size)
    // shift L back

    uint64_t current_block = (current_addr / block_size) * block_size;
    for (int64_t i = 1; i <= prefetch_count; i++){
        uint64_t next_block = current_block + block_size * i;
        prefetches.push_back(next_block);
    }

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
 ** Behavior: {
        0: null state (only at initial!!!)
        1: after 1st ever access (CLAMP BOTTOM HERE)
        2: transient
        3: two successive equal strides --> prefetch!
 ** }
 */
std::vector<uint64_t> StridePrefetcher::calculatePrefetch(uint64_t current_addr, bool miss) {
    uint32_t threshold = 1; // maximum confidence value, 1 is best for our trace!
    int64_t prefetch_count = 2; // TODO might prefetch same twice

    //* calculate address with no bottom bits
    uint64_t current_block = (current_addr / block_size) * block_size;

    // first time access
    if (!this->has_last_block) {
        has_last_block = true;
        last_block = current_block; // introduced prev address
        return {};
    }

    int64_t current_stride = current_block - last_block;

    // second time access
    if (has_last_block && last_stride == 0){
        last_stride = current_stride; // introduced stride
        confidence = 1;
        last_block = current_block;
        return {};
    }

    // third and all subsequent accesses 
    std::vector<uint64_t> blocks = {};
    if (last_stride == current_stride){
        
        // clamped increment
        confidence = (confidence+1 <= threshold) ? confidence+1 : threshold;

        if (confidence >= threshold){ // prefetch
            for (int64_t i = 1; i <= prefetch_count; i++){
                uint64_t new_block = current_block + (i*last_stride);
                blocks.push_back(new_block);
            }
        }
    }
    else {
        last_stride = current_stride;

        // pattern broken
        confidence = 0;
    }

    last_block = current_block; // update
    return blocks; // may be {}

    // // TODO: Task 3
    // Suggested design:
    // 1. Track the previous accessed block.
    // 2. Compute the current stride in block units.
    // 3. If the same stride repeats enough times, prefetch the next block at that stride.
    // 4. Update last_block / last_stride / confidence.
}

/**
 * @return vector of addresses to prefetch
 ** CacheLevel will install them to the cache
 * @param current_addr that triggered prefetch
 * @param miss only triggered on miss? IDK
 */
std::vector<uint64_t> RegionPrefetcher::calculatePrefetch(uint64_t current_addr, bool miss) {
    // (void)current_addr;
    // (void)miss;
    std::vector<uint64_t> prefetches;
    int64_t prefetch_count = 64; // how many next blocks to prefetch, default is 1, literally next line

    uint64_t current_block = (current_addr / block_size) * block_size;
    for (int64_t i = -prefetch_count; i <= prefetch_count; i++) {
        if (i == 0) continue;
        uint64_t next_block = current_block + block_size * i;
        prefetches.push_back(next_block);
    }

    return prefetches;
}


Prefetcher* createPrefetcher(std::string name, uint32_t block_size) {
    if (name == "NextLine") return new NextLinePrefetcher(block_size);
    if (name == "Stride") return new StridePrefetcher(block_size);
    if (name == "Region") return new RegionPrefetcher(block_size);
    return new NoPrefetcher();
}
