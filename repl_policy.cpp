#include "repl_policy.h"

// =========================================================
// TODO: Task 1 / Task 3 replacement policies
// Implement LRU first, then extend with SRRIP / BIP.
// =========================================================

/**
 * In the case of a cache hit (is in the cache),
 * mark the specified cache line as most recently used.
 * @param way index of the set
 */
void LRUPolicy::onHit(std::vector<CacheLine>& set, int way, uint64_t cycle) {
    if (static_cast<size_t>(way) >= set.size()) return;
    set[way].last_access = cycle;
    
    // (void)set;
    // (void)way;
    // (void)cycle;
    // // TODO: mark the hit line as most recently used.
}

/**
 * In the case of a cache miss (not in the cache), 
 * initialize a new cache line in the set for it.
 * @param way index of the set
 */
void LRUPolicy::onMiss(std::vector<CacheLine>& set, int way, uint64_t cycle) {
    if (static_cast<size_t>(way) >= set.size()) return;
    set[way].last_access = cycle; // TODO what to assign it????
    /*
     * if last_access = cycle: the AMAT is 79 cycles that matches the pdf on BB
     * if last_access = 0: suddenly the AMAT is 54 cycles LMAOOOOO
     * bry help 
     */

    // (void)set;
    // (void)way;
    // (void)cycle;
    // // TODO: initialize a newly inserted line as MRU.
}

/**
 * Get the least (oldest) recently used cache line 
 * indicated by the last access cycle timestamp.
 * @return way index of the victim
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
    if (static_cast<size_t>(way) >= set.size()) return;
    set[way].rrpv = 0;

    // (void)set;
    // (void)way;
    // (void)cycle;
    // // TODO: typically promote the line to RRPV=0.
}

void SRRIPPolicy::onMiss(std::vector<CacheLine>& set, int way, uint64_t cycle) {
    if (static_cast<size_t>(way) >= set.size()) return;
    set[way].rrpv = 2;

    // (void)set;
    // (void)way;
    // (void)cycle;
    // // TODO: insert with a long re-reference interval, e.g. RRPV=2.
}

/**
 * @return way index of victim
 */
int SRRIPPolicy::getVictim(std::vector<CacheLine>& set) {
    // TODO i hope this works
    while (true) {
        for (size_t i = 0; i < set.size(); i++) {
            // prioritize invalid ones first
            if (!set[i].valid || set[i].rrpv == 3) return i; 
        }
        for (size_t i = 0; i < set.size(); i++) {
            set[i].rrpv = std::min(3, set[i].rrpv + 1); // increment but clamp at 3
        }
    }

    // (void)set;
    // // TODO: search for RRPV==3, otherwise age all lines and retry.
    // return 0;
}

/**
 * Mark a hit cache line with by setting
 * its RRPV (Re-Reference Prediction Value) to 0,
 * which indicates it is recently used.
 * @param set cache set
 * @param way index of target cache line
 * @param cycle the current execution cycle
 */
void BIPPolicy::onHit(std::vector<CacheLine>& set, int way, uint64_t cycle) {
    if (static_cast<size_t>(way) >= set.size()) return;
    set[way].rrpv = 0; // equivalent to being MRU
    set[way].last_access = cycle; // why not, idk if used
    
    // (void)set;
    // (void)way;
    // (void)cycle;
    // // TODO: hits still become MRU.
}

/**
 * On a cache miss, set the provided cache line
 * to either have RRPV = 0 (recently used, low probability) or RRPV = 2 (old, high probability)
 * @param set cache set
 * @param way index of target cache line
 * @param cycle the current execution cycle
 */
void BIPPolicy::onMiss(std::vector<CacheLine>& set, int way, uint64_t cycle) {
    insertion_counter = (insertion_counter + 1) % throttle;
    if (static_cast<size_t>(way) >= set.size()) return;

    // for insertion_counter not equal to throttle we insert as LRU
    // otherwise MRU
    if (insertion_counter != 0) {
        set[way].rrpv = 2; // as LRU with high prob
    }
    else {
        set[way].rrpv = 0; // as MRU
    }

    set[way].last_access = cycle;

    // (void)set;
    // (void)way;
    // (void)cycle;
    // TODO: mostly insert at LRU position, but occasionally insert at MRU.
    // Hint: use insertion_counter and throttle.
}

/**
 * Get the index of the cache line to evict
 * @param set cache set
 * @return index of the victim in set
 */
int BIPPolicy::getVictim(std::vector<CacheLine>& set) {
    // i don't know why the same as LRU
    // if same, then i have to use last_access timestamp
    // but then i have to make the last_access as zero everytime for LRU

    // ill implement like SRRIP instead
    // but actually it's the same cuz we still try to get the LRU
    for (size_t i = 0; i < set.size(); i++) {
        if (!set[i].valid || set[i].rrpv == 3) return i; // prioritize invalid ones first
    }
    for (size_t i = 0; i < set.size(); i++) {
        set[i].rrpv = std::min(set[i].rrpv + 1, 3);
    }

    return getVictim(set);

    // (void)set;
    // // TODO: BIP usually uses the same victim selection as LRU.
    // return 0;
}

ReplacementPolicy* createReplacementPolicy(std::string name) {
    if (name == "SRRIP") return new SRRIPPolicy();
    if (name == "BIP") return new BIPPolicy();
    return new LRUPolicy();
}
