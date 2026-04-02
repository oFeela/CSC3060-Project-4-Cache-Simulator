# CSC3060 Project 4 - Cache Simulator
The fourth project of CSC3060 2026.

## Task Split - No Dependencies (I hope) Between Persons
### Task 1
Person A
- Implement get_index() in memory_hierarchy.cpp

- Implement get_tag() in memory_hierarchy.cpp

- Implement hit detection logic in CacheLevel::access()

- Implement miss handling (victim selection, write-back, fetch, install) in CacheLevel::access()

Person B
- Implement reconstruct_addr() in memory_hierarchy.cpp

- Implement LRU::get_victim() in repl_policy.cpp

- Implement LRU::update_replacement_state() in repl_policy.cpp

- Implement LRU::invalidate() in repl_policy.cpp

### Task 2
Person A
- Add --enable-l2 flag parsing in main.cpp

- Create L2 CacheLevel object in main.cpp

- Connect L1 next_level pointer to L2 in main.cpp

- Set L2 next_level pointer to Main Memory in main.cpp

Person B
- Add L2 access counter variable in memory_hierarchy.cpp

- Add L2 hit counter variable in memory_hierarchy.cpp

- Add L2 miss counter variable in memory_hierarchy.cpp

- Add L2 write-back counter variable in memory_hierarchy.cpp

- Print L2 statistics in simulation output in memory_hierarchy.cpp

### Task 3
Person A
- Add RRPV field to CacheLine struct in defs.h

- Implement SRRIP::get_victim() in repl_policy.cpp

- Implement SRRIP::update_replacement_state() in repl_policy.cpp

- Implement SRRIP::invalidate() in repl_policy.cpp

- Implement StridePrefetcher::prefetch() in prefetcher.cpp

Person B
- Implement BIP::get_victim() in repl_policy.cpp

- Implement BIP::update_replacement_state() in repl_policy.cpp

- Implement BIP::invalidate() in repl_policy.cpp

- Implement NextLinePrefetcher::prefetch() in prefetcher.cpp

- Implement install_prefetch() in memory_hierarchy.cpp

- Call prefetcher from CacheLevel::access() in memory_hierarchy.cpp