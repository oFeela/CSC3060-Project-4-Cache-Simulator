#include "repl_policy.h"

// =========================================================
// TODO: Task 1 / Task 3 replacement policies
// Implement LRU first, then extend with SRRIP / BIP.
// =========================================================

/**
 * In the case of a cache hit (is in the cache),
 * mark the specified cache line as most recently used.
 */
void LRUPolicy::onHit(std::vector<CacheLine>& set, int way, uint64_t cycle) {
    if (way >= set.size()) return;
    set[way].last_access = cycle;
    
    // (void)set;
    // (void)way;
    // (void)cycle;
    // // TODO: mark the hit line as most recently used.
}

/**
 * In the case of a cache miss (not in the cache), 
 * initialize a new cache line in the set for it.
 */
void LRUPolicy::onMiss(std::vector<CacheLine>& set, int way, uint64_t cycle) {
    if (way >= set.size()) return;
    set[way].last_access = cycle;

    // (void)set;
    // (void)way;
    // (void)cycle;
    // // TODO: initialize a newly inserted line as MRU.
}

/**
 * Get the least (oldest) recently used cache line 
 * indicated by the last access cycle timestamp.
 */
int LRUPolicy::getVictim(std::vector<CacheLine>& set) {
    // always prioritize invalid cache line
    // find the one with min last access
    // on equal scenario, prioritize the invalid one

    uint64_t min_last_access = UINT64_MAX;
    int ans_index = 0;

    for (size_t i = 0; i < set.size(); i++) {
        CacheLine& line = set[i];
        if (!line.valid) return i;
        if (line.last_access < min_last_access) {
            min_last_access = line.last_access;
            ans_index = i;
        }
    }

    return ans_index;

    // (void)set;
    // // TODO: return the least recently used way.

    // return 0;
}

void SRRIPPolicy::onHit(std::vector<CacheLine>& set, int way, uint64_t cycle) {
    (void)set;
    (void)way;
    (void)cycle;
    // TODO: typically promote the line to RRPV=0.
}

void SRRIPPolicy::onMiss(std::vector<CacheLine>& set, int way, uint64_t cycle) {
    (void)set;
    (void)way;
    (void)cycle;
    // TODO: insert with a long re-reference interval, e.g. RRPV=2.
}

int SRRIPPolicy::getVictim(std::vector<CacheLine>& set) {
    (void)set;
    // TODO: search for RRPV==3, otherwise age all lines and retry.
    return 0;
}

void BIPPolicy::onHit(std::vector<CacheLine>& set, int way, uint64_t cycle) {
    (void)set;
    (void)way;
    (void)cycle;
    // TODO: hits still become MRU.
}

void BIPPolicy::onMiss(std::vector<CacheLine>& set, int way, uint64_t cycle) {
    (void)set;
    (void)way;
    (void)cycle;
    // TODO: mostly insert at LRU position, but occasionally insert at MRU.
    // Hint: use insertion_counter and throttle.
}

int BIPPolicy::getVictim(std::vector<CacheLine>& set) {
    (void)set;
    // TODO: BIP usually uses the same victim selection as LRU.
    return 0;
}

ReplacementPolicy* createReplacementPolicy(std::string name) {
    if (name == "SRRIP") return new SRRIPPolicy();
    if (name == "BIP") return new BIPPolicy();
    return new LRUPolicy();
}
